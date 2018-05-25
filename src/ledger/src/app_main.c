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
#include "ui.h"
#include "app_main.h"

#include "apdu_codes.h"
#include "xmss.h"
#include "nvram.h"

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

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

    case SEPROXYHAL_TAG_TICKER_EVENT:   //
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

void app_init()
{
    io_seproxyhal_init();
    USB_power(0);
    USB_power(1);
    ui_idle();
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
#endif

xmss_sig_ctx_t ctx;

#ifdef TESTING_ENABLED
void test_write_leaf(volatile uint32_t *tx, uint32_t rx)
{
    if (rx!=2+1+32)
    {
        THROW(APDU_CODE_UNKNOWN);
    }
    const uint8_t index = G_io_apdu_buffer[2];
    const uint8_t *p=N_DATA.xmss_nodes + 32 * index;
    nvcpy(p, G_io_apdu_buffer+3, 32);

    {
        char buffer[40];
        snprintf(buffer, 40, "Write Leaf %d", ctx.sig_idx);
        debug_printf(buffer);
    }
}

void test_read_leaf(volatile uint32_t *tx, uint32_t rx)
{
    if (rx!=2+1)
    {
        THROW(APDU_CODE_UNKNOWN);
    }
    const uint8_t index = G_io_apdu_buffer[2];
    const uint8_t *p=N_DATA.xmss_nodes + 32 * index;

    os_memmove(G_io_apdu_buffer, p, 32);
    {
        char buffer[40];
        snprintf(buffer, 40, "Read Leaf %d", index+1);
        debug_printf(buffer);
    }

    *tx+=32;
}

void test_digest(volatile uint32_t *tx, uint32_t rx)
{
    if (rx!=2+1+32)
    {
        THROW(APDU_CODE_UNKNOWN);
    }

    xmss_gen_keys_1_get_seeds(&N_DATA.sk, test_seed);

    xmss_digest_t digest;
    memset(digest.raw, 0, XMSS_DIGESTSIZE);

    const uint8_t index = G_io_apdu_buffer[2];
    const uint8_t *msg=G_io_apdu_buffer+3;

    xmss_digest(&digest, msg, &N_DATA.sk, index);

    {
        char buffer[40];
        snprintf(buffer, 40, "Digest idx %d", index+1);
        debug_printf(buffer);
    }

    os_memmove(G_io_apdu_buffer, digest.raw, 64);

    *tx+=64;
}

void test_sign(volatile uint32_t *tx, uint32_t rx)
{
    if (rx!=2+1+32)
    {
        THROW(APDU_CODE_UNKNOWN);
    }

    const uint8_t index = G_io_apdu_buffer[2];
    const uint8_t *msg=G_io_apdu_buffer+3;

    xmss_sign_incremental_init(&ctx, msg, &N_DATA.sk, index);

    debug_printf("Sign1");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign2");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign3");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign4");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign5");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign6");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign7");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign8");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("Sign9");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);
    debug_printf("SignA");  xmss_sign_incremental(&ctx, G_io_apdu_buffer, &N_DATA.sk, N_DATA.xmss_nodes, index);

    if (ctx.written>0) {
        *tx += ctx.written;
    }
}

#endif

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
                    G_io_apdu_buffer[0] = VERSION_TESTING;
                    G_io_apdu_buffer[1] = LEDGER_MAJOR_VERSION;
                    G_io_apdu_buffer[2] = LEDGER_MINOR_VERSION;
                    G_io_apdu_buffer[3] = LEDGER_PATCH_VERSION;
                    tx += 4;

                    LOGSTACK();

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
                        if (rx<4)
                        {
                            THROW(APDU_CODE_UNKNOWN);
                        }
                        xmss_gen_keys_1_get_seeds(&N_DATA.sk, test_seed);

                        const uint16_t idx = (G_io_apdu_buffer[2]<<8u)+G_io_apdu_buffer[3];
                        const uint8_t *p=N_DATA.xmss_nodes + 32 * idx;

                        xmss_gen_keys_2_get_nodes((uint8_t*) &N_DATA.wots_buffer, p, &N_DATA.sk, idx);

                        os_memmove(G_io_apdu_buffer, p, 32);
                        tx+=32;
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_PK: {
                        xmss_pk_t pk;
                        memset(pk.raw, 0, 64);

                        xmss_gen_keys_3_get_root(N_DATA.xmss_nodes, &N_DATA.sk);
                        xmss_pk(&pk, &N_DATA.sk );

                        os_memmove(G_io_apdu_buffer, pk.raw, 64);
                        tx+=64;
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_WRITE_LEAF: {
                        test_write_leaf(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_READ_LEAF: {
                        test_read_leaf(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_DIGEST: {
                        test_digest(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
                    }

                    case INS_TEST_SIGN: {
                        test_sign(&tx, rx);
                        THROW(APDU_CODE_OK);
                        break;
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

