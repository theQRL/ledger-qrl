/*******************************************************************************
*   (c) 2016 Ledger
*   (c) 2018 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include <os_io_seproxyhal.h>
#include "os.h"
#include "view.h"
#include "storage.h"
#include "app_main.h"

#include "apdu_codes.h"
#include "xmss.h"
#include "nvram.h"
#include "storage.h"

#define CONDITIONAL_REDISPLAY  { if (UX_ALLOWED) UX_REDISPLAY() };

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];
xmss_sig_ctx_t ctx;

unsigned char io_event(unsigned char channel)
{
    switch (G_io_seproxyhal_spi_buffer[0]) {
    case SEPROXYHAL_TAG_FINGER_EVENT: //
        UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
        break;

    case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT: // for Nano S
        UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
        break;

    case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
        if (!UX_DISPLAYED())
            UX_DISPLAYED_EVENT();
        break;

    case SEPROXYHAL_TAG_TICKER_EVENT:UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, CONDITIONAL_REDISPLAY);
        break;

        // unknown events are acknowledged
    default:UX_DEFAULT_EVENT();
        break;
    }
    if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
    }
    return 1; // DO NOT reset the current APDU transport
}

unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len)
{
    switch (channel & ~(IO_FLAGS)) {
    case CHANNEL_KEYBOARD:break;

        // multiplexed io exchange over a SPI channel and TLV encapsulated protocol
    case CHANNEL_SPI:
        if (tx_len) {
            io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

            if (channel & IO_RESET_AFTER_REPLIED) {
                reset();
            }
            return 0; // nothing received from the master so far (it's a tx
            // transaction)
        }
        else {
            return io_seproxyhal_spi_recv(G_io_apdu_buffer,
                    sizeof(G_io_apdu_buffer), 0);
        }

    default:THROW(INVALID_PARAMETER);
    }
    return 0;
}

////////////////////////////////////////////////
////////////////////////////////////////////////

void app_init()
{
    io_seproxyhal_init();
    USB_power(0);
    USB_power(1);

    view_update_state(100);
    view_idle();

    memset(&ctx, 0, sizeof(xmss_sig_ctx_t));
}

#define VERSION_TESTING 0x00
#ifdef TESTING_ENABLED
const uint8_t test_seed[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#undef VERSION_TESTING
#define VERSION_TESTING 0xFF

void test_set_state(volatile uint32_t *tx, uint32_t rx)
{
    if (rx<5)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    nvcpy((void*)N_appdata.raw, G_io_apdu_buffer+2, 3);

    view_update_state(500);
}

void test_pk_gen2(volatile uint32_t *tx, uint32_t rx)
{
    if (rx<5)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    const uint16_t idx = (p1<<8u)+p2;
    const uint8_t *p=N_DATA.xmss_nodes + 32 * idx;

    xmss_gen_keys_1_get_seeds(&N_DATA.sk, test_seed);
    xmss_gen_keys_2_get_nodes((uint8_t*) &N_DATA.wots_buffer, (void*)p, &N_DATA.sk, idx);

    os_memmove(G_io_apdu_buffer, p, 32);
    *tx+=32;

    view_update_state(500);
}

void test_keygen(volatile uint32_t* tx, uint32_t rx)
{
    if (rx<5) {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    keygen();

    G_io_apdu_buffer[0] = N_appdata.mode;
    G_io_apdu_buffer[1] = N_appdata.xmss_index >> 8;
    G_io_apdu_buffer[2] = N_appdata.xmss_index & 0xFF;
    *tx += 3;

    view_update_state(500);
}

void test_write_leaf(volatile uint32_t *tx, uint32_t rx)
{
    if (rx<5+32 || (rx-5)%32!=0)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }

    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    const uint8_t size = rx-4;
    const uint8_t index = p1;
    const uint8_t *p=N_DATA.xmss_nodes + 32 * index;

    snprintf(ui_buffer, sizeof(ui_buffer), "W[%03d]: %03d", size, index);
    debug_printf(ui_buffer);

    nvcpy((void*)p, data, size);
    view_update_state(2000);
}

void test_calc_pk(volatile uint32_t *tx, uint32_t rx)
{
    if (rx<5)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }

    snprintf(ui_buffer, sizeof(ui_buffer), "keygen: root");
    debug_printf(ui_buffer);

    xmss_pk_t pk;
    memset(pk.raw, 0, 64);

    xmss_gen_keys_3_get_root(N_DATA.xmss_nodes, &N_DATA.sk);
    xmss_pk(&pk, &N_DATA.sk);

    nvm_write(N_appdata.pk.raw, pk.raw, 64);

    appstorage_t tmp;
    tmp.mode = APPMODE_READY;
    tmp.xmss_index = 0;
    nvm_write((void*) &N_appdata.raw, &tmp.raw, sizeof(tmp.raw));

    view_update_state(50);
}

void test_read_leaf(volatile uint32_t *tx, uint32_t rx)
{
    if (rx<5)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    const uint8_t index = p1;
    const uint8_t *p=N_DATA.xmss_nodes + 32 * index;

    os_memmove(G_io_apdu_buffer, p, 32);

    snprintf(ui_buffer, sizeof(ui_buffer), "Read %d", index);
    debug_printf(ui_buffer);

    *tx+=32;
    view_update_state(2000);
}

void test_digest(volatile uint32_t *tx, uint32_t rx)
{
    if (rx!=5+32)
    {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t *data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    xmss_gen_keys_1_get_seeds(&N_DATA.sk, test_seed);

    xmss_digest_t digest;
    memset(digest.raw, 0, XMSS_DIGESTSIZE);

    const uint8_t index = p1;
    xmss_digest(&digest, data, &N_DATA.sk, index);

    snprintf(ui_buffer, sizeof(ui_buffer), "Digest idx %d", index+1);
    debug_printf(ui_buffer);

    os_memmove(G_io_apdu_buffer, digest.raw, 64);

    *tx+=64;
    view_update_state(2000);
}

#endif

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void app_get_version(volatile uint32_t* tx, uint32_t rx)
{
    if (rx<5) {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t* data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    G_io_apdu_buffer[0] = VERSION_TESTING;
    G_io_apdu_buffer[1] = LEDGER_MAJOR_VERSION;
    G_io_apdu_buffer[2] = LEDGER_MINOR_VERSION;
    G_io_apdu_buffer[3] = LEDGER_PATCH_VERSION;
    *tx += 4;

    snprintf(ui_buffer, sizeof(ui_buffer),
            "Ver %02d.%02d.%02d",
            LEDGER_MAJOR_VERSION,
            LEDGER_MINOR_VERSION,
            LEDGER_PATCH_VERSION);
    debug_printf(ui_buffer);

    view_update_state(2000);
}

void app_get_state(volatile uint32_t* tx, uint32_t rx)
{
    if (rx<5) {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t* data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    G_io_apdu_buffer[0] = N_appdata.mode;
    G_io_apdu_buffer[1] = N_appdata.xmss_index >> 8;
    G_io_apdu_buffer[2] = N_appdata.xmss_index & 0xFF;
    *tx += 3;

    view_update_state(500);
}

void get_seed(uint8_t* seed)
{
    // based on RFC8032, the private key is 32 octets (256 bits, corresponding to b) of
    // cryptographically secure random data.
    uint8_t privateKeyData1[32];
    uint8_t privateKeyData2[32];

    uint32_t bip32_path[5] = {
            0x80000000 | 44,
            0x80000000 | 60,
            0x80000000 | 0,
            0x80000000 | 0,
            0x80000000 | 0
    };

    os_perso_derive_node_bip32(CX_CURVE_Ed25519, bip32_path, 5, privateKeyData1, NULL);
    bip32_path[2]++;
    os_perso_derive_node_bip32(CX_CURVE_Ed25519, bip32_path, 5, privateKeyData2, NULL);

    memcpy(seed, privateKeyData1, 32);
    memcpy(seed+32, privateKeyData2, 16);
}

bool keygen()
{
    if (N_appdata.mode!=APPMODE_NOT_INITIALIZED && N_appdata.mode!=APPMODE_KEYGEN_RUNNING) {
        return false;
    }

    appstorage_t tmp;

    if (N_appdata.mode==APPMODE_NOT_INITIALIZED) {
        uint8_t seed[48];

        get_seed(seed);

        xmss_gen_keys_1_get_seeds(&N_DATA.sk, seed);

        tmp.mode = APPMODE_KEYGEN_RUNNING;
        tmp.xmss_index = 0;

        nvm_write((void*) &N_appdata.raw, &tmp.raw, sizeof(tmp.raw));
    }

    if (N_appdata.xmss_index<256) {
        snprintf(ui_buffer, sizeof(ui_buffer), "keygen: %03d/256", N_appdata.xmss_index+1);
        debug_printf(ui_buffer);

        const uint8_t* p = N_DATA.xmss_nodes+32*N_appdata.xmss_index;
        xmss_gen_keys_2_get_nodes((uint8_t*) & N_DATA.wots_buffer, (void*) p, &N_DATA.sk, N_appdata.xmss_index);

        tmp.mode = APPMODE_KEYGEN_RUNNING;
        tmp.xmss_index = N_appdata.xmss_index+1;
    }
    else {
        snprintf(ui_buffer, sizeof(ui_buffer), "keygen: root");
        debug_printf(ui_buffer);

        xmss_pk_t pk;
        memset(pk.raw, 0, 64);

        xmss_gen_keys_3_get_root(N_DATA.xmss_nodes, &N_DATA.sk);
        xmss_pk(&pk, &N_DATA.sk);

        nvm_write(N_appdata.pk.raw, pk.raw, 64);

        tmp.mode = APPMODE_READY;
        tmp.xmss_index = 0;
    }

    nvm_write((void*) &N_appdata.raw, &tmp.raw, sizeof(tmp.raw));

    return N_appdata.mode!=APPMODE_READY;
}

void app_get_pk(volatile uint32_t* tx, uint32_t rx)
{
    if (rx<5) {
        THROW(APDU_CODE_WRONG_LENGTH);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t* data = G_io_apdu_buffer+5;

    if (N_appdata.mode!=APPMODE_READY) {
        THROW(APDU_CODE_COMMAND_NOT_ALLOWED);
    }

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    // Add pk descriptor
    G_io_apdu_buffer[0] = 0;        // XMSS, SHA2-256
    G_io_apdu_buffer[1] = 4;        // Height 8
    G_io_apdu_buffer[2] = 0;        // SHA256_X

    os_memmove(G_io_apdu_buffer+3, N_appdata.pk.raw, 64);
    *tx += 67;

    THROW(APDU_CODE_OK);
    view_update_state(500);
}

bool parse_unsigned_message(volatile uint32_t* tx, uint32_t rx)
{
    // TODO: message uploading, parsing + view + hashing

    if (rx!=5+32) {
        THROW(APDU_CODE_WRONG_LENGTH);
    }

    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t* data = G_io_apdu_buffer+5;

    if (N_appdata.mode!=APPMODE_READY) {
        THROW(APDU_CODE_COMMAND_NOT_ALLOWED);
    }

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    const uint8_t* msg = G_io_apdu_buffer+5;
    nvm_write((void*) N_appdata.unsigned_message, (void*) msg, 32);

    return true;
}

void app_sign(volatile uint32_t* tx, uint32_t rx)
{
    const uint8_t* msg = N_appdata.unsigned_message;

    // TODO: SHA256

    // buffer[2..3] are ignored (p1, p2)
    xmss_sign_incremental_init(&ctx,
            msg,
            &N_DATA.sk,
            (uint8_t*) N_DATA.xmss_nodes,
            N_appdata.xmss_index);

    // Move index forward
    appstorage_t tmp;
    tmp.mode = APPMODE_READY;
    tmp.xmss_index = N_appdata.xmss_index+1;
    nvm_write((void*) &N_appdata.raw, &tmp.raw, sizeof(tmp.raw));

    set_code(G_io_apdu_buffer, 0, APDU_CODE_OK);
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);

    view_idle();
}

void app_sign_next(volatile uint32_t* tx, uint32_t rx)
{
    if (N_appdata.mode!=APPMODE_READY) {
        THROW(APDU_CODE_COMMAND_NOT_ALLOWED);
    }
    const uint8_t p1 = G_io_apdu_buffer[2];
    const uint8_t p2 = G_io_apdu_buffer[3];
    const uint8_t* data = G_io_apdu_buffer+5;

    UNUSED(p1);
    UNUSED(p2);
    UNUSED(data);

    const uint16_t index = N_appdata.xmss_index-1;      // It has already been updated

    if (ctx.sig_chunk_idx<10)
        xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, index);
    else
        xmss_sign_incremental_last(&ctx, G_io_apdu_buffer, &N_DATA.sk, index);

    if (ctx.written>0) {
        *tx = ctx.written;
    }

    if (ctx.sig_chunk_idx==10) {
        view_update_state(1000);
    }

    view_update_state(500);
}

void app_main()
{
    volatile uint32_t rx = 0, tx = 0, flags = 0;

    for (;;) {
        volatile uint16_t sw = 0;

        BEGIN_TRY;
        {
            TRY;
            {
                rx = tx;
                tx = 0;
                rx = io_exchange(CHANNEL_APDU | flags, rx);
                flags = 0;

                if (rx==0) THROW(0x6982);

                if (G_io_apdu_buffer[OFFSET_CLA]!=CLA) {
                    THROW(APDU_CODE_CLA_NOT_SUPPORTED);
                }

                switch (G_io_apdu_buffer[OFFSET_INS]) {

                case INS_VERSION: {
                    app_get_version(&tx, rx);
                    THROW(APDU_CODE_OK);
                    break;
                }

                case INS_GETSTATE: {
                    app_get_state(&tx, rx);
                    THROW(APDU_CODE_OK);
                    break;
                }

                case INS_PUBLIC_KEY: {
                    app_get_pk(&tx, rx);
                    THROW(APDU_CODE_OK);
                    break;
                }

                case INS_SIGN: {
                    if (N_appdata.mode!=APPMODE_READY) {
                        THROW(APDU_CODE_COMMAND_NOT_ALLOWED);
                    }
                    parse_unsigned_message(&tx, rx);
                    view_sign();
                    flags |= IO_ASYNCH_REPLY;
                    break;
                }

                case INS_SIGN_NEXT: {
                    debug_printf("SIGNING");
                    app_sign_next(&tx, rx);
                    view_update_state(1000);
                    THROW(APDU_CODE_OK);
                    break;
                }

#ifdef TESTING_ENABLED
                    case INS_TEST_PK_GEN_1: {
                        xmss_gen_keys_1_get_seeds(&N_DATA.sk, test_seed);
                        os_memmove(G_io_apdu_buffer, N_DATA.sk.raw, 132);
                        tx+=132;
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_PK_GEN_2: {
                        test_pk_gen2(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_KEYGEN: {
                        test_keygen(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_WRITE_LEAF: {
                        test_write_leaf(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_CALC_PK: {
                        test_calc_pk(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_READ_LEAF: {
                        test_read_leaf(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_SETSTATE: {
                        test_set_state(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_DIGEST: {
                        test_digest(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_COMM:
                    {
                        uint8_t count = G_io_apdu_buffer[2];
                        for (int i = 0; i < count; i++)
                            {
                                G_io_apdu_buffer[i] = 1+i;
                                tx++;
                            }
                        THROW(APDU_CODE_OK);
                    }
#endif
                default: {
                    THROW(APDU_CODE_INS_NOT_SUPPORTED);
                }

                }
            }
            CATCH(EXCEPTION_IO_RESET)
            {
                THROW(EXCEPTION_IO_RESET);
            }
            CATCH_OTHER(e);
            {
                switch (e & 0xF000) {
                case 0x6000:
                case APDU_CODE_OK:sw = e;
                    break;
                default:sw = 0x6800 | (e & 0x7FF);
                    break;
                }
                G_io_apdu_buffer[tx] = sw >> 8;
                G_io_apdu_buffer[tx+1] = sw;
                tx += 2;
            }
            FINALLY;
            { }
        }
        END_TRY;
    }
}

