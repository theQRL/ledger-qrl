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
#include <string.h>
#include "glyphs.h"
#include "app.h"
#include "view.h"
#include "app_main.h"
#include "apdu_codes.h"
#include "storage.h"
#include "view_templates.h"
#include "app_types.h"
#include <zxio.h>

ux_state_t ux;
enum UI_STATE view_uiState;

char view_title[16];
char view_buffer_key[MAX_CHARS_PER_KEY_LINE];
char view_buffer_value[MAX_CHARS_PER_VALUE_LINE];
int8_t view_idx;

#define COND_SCROLL_L2 0xF0

////////////////////////////////////////////////
//------ View elements

const ux_menu_entry_t menu_main[];
const ux_menu_entry_t menu_about[];
const ux_menu_entry_t menu_sign[];

const ux_menu_entry_t menu_sign[] = {
    {NULL, handler_view_tx, 0, NULL, "View transaction", NULL, 0, 0},
    {NULL, handler_sign_tx, 0, NULL, "Sign transaction", NULL, 0, 0},
    {NULL, handler_reject_tx, 0, &C_icon_back, "Reject", NULL, 60, 40},
    UX_MENU_END
};

const ux_menu_entry_t menu_main[] = {
#if TESTING_ENABLED
    {NULL, NULL, 0, &C_icon_app, "QRL TEST", view_buffer_value, 32, 11},
#else
    {NULL, NULL, 0, &C_icon_app, "QRL", view_buffer_value, 32, 11},
#endif
    {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
    {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END
};

const ux_menu_entry_t menu_main_not_ready[] = {
#if TESTING_ENABLED
    {NULL, NULL, 0, &C_icon_app, "QRL TEST", view_buffer_value, 32, 11},
#else
    {NULL, NULL, 0, &C_icon_app, "QRL", view_buffer_value, 32, 11},
#endif
    {NULL, handler_init_device, 0, NULL, "Init Device", NULL, 0, 0},
    {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
    {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
    UX_MENU_END
};

const ux_menu_entry_t menu_about[] = {
    {menu_main, NULL, 0, &C_icon_back, "Version", APPVERSION, 0, 0},
    UX_MENU_END
};

static const bagl_element_t view_txinfo[] = {
    UI_FillRectangle(0, 0, 0, 128, 32, 0x000000, 0xFFFFFF),
    UI_Icon(0, 0, 0, 7, 7, BAGL_GLYPH_ICON_LEFT),
    UI_Icon(0, 128 - 7, 0, 7, 7, BAGL_GLYPH_ICON_RIGHT),
    UI_LabelLine(1, 0, 8, 128, 11, 0xFFFFFF, 0x000000, (const char *) view_title),
    UI_LabelLine(1, 0, 19, 128, 11, 0xFFFFFF, 0x000000, (const char *) view_buffer_key),
    UI_LabelLineScrolling(2, 0, 30, 128, 11, 0xFFFFFF, 0x000000, (const char *) view_buffer_value),
};

void io_seproxyhal_display(const bagl_element_t *element) {
    io_seproxyhal_display_default((bagl_element_t *) element);
}

const bagl_element_t *menu_main_prepro(const ux_menu_entry_t *menu_entry, bagl_element_t *element) {
    switch (menu_entry->userid) {
    case COND_SCROLL_L2: {
        switch (element->component.userid) {
        case 0x21: // 1st line
            // element->component.font_id = BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER;
            break;
        case 0x22: // 2nd line
            element->component.stroke = 1;
            element->component.width = 80;
            element->component.icon_id = 50;
            break;
        }
    }
    }
    return element;
}

////////////////////////////////////////////////
//------ Event handlers

void view_txinfo_show();
static unsigned int view_txinfo_button(unsigned int button_mask,
                                       unsigned int button_mask_counter) {
    switch (button_mask) {
        // Press both left and right buttons to quit
    case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: {
        view_sign_menu();
        break;
    }

        // Press left to progress to the previous element
    case BUTTON_EVT_RELEASED | BUTTON_LEFT: {
        view_idx--;
        view_txinfo_show();
        break;
    }

        // Press right to progress to the next element
    case BUTTON_EVT_RELEASED | BUTTON_RIGHT: {
        view_idx++;
        view_txinfo_show();
        break;
    }

    }
    return 0;
}

const bagl_element_t *view_txinfo_prepro(const bagl_element_t *element) {

    switch (element->component.userid) {
    case 0x01:UX_CALLBACK_SET_INTERVAL(2000);
        break;
    case 0x02:UX_CALLBACK_SET_INTERVAL(MAX(3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
        break;
    case 0x03:UX_CALLBACK_SET_INTERVAL(MAX(3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
        break;
    }
    return element;
}

#define ARRTOHEX(X, Y) array_to_hexstr(X, Y, sizeof(Y))
#define AMOUNT_TO_STR(OUTPUT, AMOUNT, DECIMALS) fpuint64_to_str(OUTPUT, uint64_from_BEarray(AMOUNT), DECIMALS)

uint64_t uint64_from_BEarray(const uint8_t data[8]) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result <<= 8;
        result += data[i];
    }
    return result;
}

void view_txinfo_show() {
#define EXIT_VIEW() {view_sign_menu(); return;}
#define PTR_DIST(p2, p1) ((char *)p2) - ((char *)p1)

    if (view_idx < 0 || view_idx > 50) {
        EXIT_VIEW();
    }

    // TODO: Validate TX minimum SIZES

    uint8_t
    elem_idx = 0;

    switch (ctx.qrltx.type) {

    case QRLTX_TX: {
        strcpy(view_title, "TRANSFER");

        switch (view_idx) {
        case 0: {
            strcpy(view_buffer_key, "src addr");
            ARRTOHEX(view_buffer_value, ctx.qrltx.tx.master.address);
            break;
        }
        case 1: {
            strcpy(view_buffer_key, "fee (quanta)");
            AMOUNT_TO_STR(view_buffer_value, ctx.qrltx.tx.master.amount, QUANTA_DECIMALS);
            break;
        }
        default: {
            elem_idx = (view_idx - 2) >> 1;
            if (elem_idx > QRLTX_SUBITEM_MAX) EXIT_VIEW();
            if (elem_idx > ctx.qrltx.subitem_count) EXIT_VIEW();

            qrltx_addr_block *dst = &ctx.qrltx.tx.dst[elem_idx];

            if (elem_idx % 2 == 0) {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "dst %d", elem_idx);
                ARRTOHEX(view_buffer_value, dst->address);
            } else {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "amount %d", elem_idx);
                AMOUNT_TO_STR(view_buffer_value, dst->amount, QUANTA_DECIMALS);
            }
            break;
        }
        }
    }
    case QRLTX_TXTOKEN: {
        strcpy(view_title, "TRANSFER TOKEN");

        switch (view_idx) {
        case 0: {
            strcpy(view_buffer_key, "src addr");
            ARRTOHEX(view_buffer_value, ctx.qrltx.txtoken.master.address);
            break;
        }
        case 1: {
            strcpy(view_buffer_key, "fee (quanta)");
            AMOUNT_TO_STR(view_buffer_value, ctx.qrltx.txtoken.master.amount, QUANTA_DECIMALS);
            break;
        }
        case 2: {
            strcpy(view_buffer_key, "token hash");
            ARRTOHEX(view_buffer_value, ctx.qrltx.txtoken.token_hash);
            break;
        }
        default: {
            elem_idx = (view_idx - 3) >> 2;
            if (elem_idx > QRLTX_SUBITEM_MAX) EXIT_VIEW();
            if (elem_idx > ctx.qrltx.subitem_count) EXIT_VIEW();

            qrltx_addr_block *dst = &ctx.qrltx.txtoken.dst[elem_idx];

            if (elem_idx % 2 == 0) {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "dst %d", elem_idx);
                ARRTOHEX(view_buffer_value, dst->address);
            } else {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "amount %d", elem_idx);
                // TODO: Decide what to do with token decimals
                AMOUNT_TO_STR(view_buffer_value, dst->amount, 0);
            }
            break;
        }
        }
        break;
    }
    case QRLTX_SLAVE: {
        strcpy(view_title, "CREATE SLAVE");

        switch (view_idx) {
        case 0: {
            strcpy(view_buffer_key, "master addr");
            ARRTOHEX(view_buffer_value, ctx.qrltx.slave.master.address);
            break;
        }
        case 1: {
            strcpy(view_buffer_key, "fee (quanta)");
            AMOUNT_TO_STR(view_buffer_value, ctx.qrltx.slave.master.amount, QUANTA_DECIMALS);
            break;
        }
        default: {
            elem_idx = (view_idx - 2) >> 1;
            if (elem_idx > QRLTX_SUBITEM_MAX) EXIT_VIEW();
            if (elem_idx > ctx.qrltx.subitem_count) EXIT_VIEW();

            qrltx_slave_block *dst = &ctx.qrltx.slave.slaves[elem_idx];
            if (PTR_DIST(dst, &ctx.qrltx.slave) > ctx.qrltx.payload_size) EXIT_VIEW();

            if (elem_idx % 2 == 0) {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "slave pk %d", elem_idx);
                ARRTOHEX(view_buffer_value, ctx.qrltx.slave.slaves[elem_idx].pk);
            } else {
                snprintf(view_buffer_key, sizeof(view_buffer_key), "access type %d", elem_idx);
                ARRTOHEX(view_buffer_value, ctx.qrltx.slave.slaves[elem_idx].access);
            }
            break;
        }
        }
        break;
    }

    }

    UX_DISPLAY(view_txinfo, view_txinfo_prepro);
}

void handler_init_device(unsigned int unused) {
    UNUSED(unused);
//  UX_DISPLAY(bagl_ui_keygen, NULL);
    while (keygen()) {
        view_update_state(50);
    }
    view_update_state(50);
}

void handler_view_tx(unsigned int unused) {
    UNUSED(unused);
//    debug_printf("Not implemented");
//    view_update_state(2000);

    view_idx = 0;
    view_txinfo_show();
}

void handler_sign_tx(unsigned int unused) {
    UNUSED(unused);
    app_sign();
    view_update_state(2000);
}

void handler_reject_tx(unsigned int unused) {
    UNUSED(unused);
    set_code(G_io_apdu_buffer, 0, APDU_CODE_COMMAND_NOT_ALLOWED);
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    view_main_menu();
}

////////////////////////////////////////////////
////////////////////////////////////////////////

void view_init(void) {
    UX_INIT();
    view_uiState = UI_IDLE;
}

void view_main_menu(void) {
    view_uiState = UI_IDLE;

    if (N_appdata.mode != APPMODE_READY) {
        UX_MENU_DISPLAY(0, menu_main_not_ready, menu_main_prepro);
    } else {
        UX_MENU_DISPLAY(0, menu_main, menu_main_prepro);
    }
}

void view_sign_menu(void) {
    view_uiState = UI_SIGN;
    UX_MENU_DISPLAY(0, menu_sign, NULL);
}

void view_update_state(uint16_t interval) {
    switch (N_appdata.mode) {
    case APPMODE_NOT_INITIALIZED: {
        snprintf(view_buffer_value, sizeof(view_buffer_value), "not ready ");
    }
        break;
    case APPMODE_KEYGEN_RUNNING: {
        snprintf(view_buffer_value, sizeof(view_buffer_value), "KEYGEN rem:%03d", 256 - N_appdata.xmss_index);
    }
        break;
    case APPMODE_READY: {
        if (N_appdata.xmss_index > 250) {
            snprintf(view_buffer_value, sizeof(view_buffer_value), "WARN! rem:%03d", 256 - N_appdata.xmss_index);
        } else {
            snprintf(view_buffer_value, sizeof(view_buffer_value), "READY rem:%03d", 256 - N_appdata.xmss_index);
        }
    }
        break;
    }
    UX_CALLBACK_SET_INTERVAL(interval);
}
