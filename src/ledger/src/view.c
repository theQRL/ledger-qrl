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

ux_state_t ux;
enum UI_STATE view_uiState;
char ui_buffer[20];

#define COND_SCROLL_L2 0xF0

volatile char infoDataKey[MAX_CHARS_PER_KEY_LINE];
volatile char infoDataValue[MAX_CHARS_PER_VALUE_LINE];


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
        {NULL, NULL, 0, &C_icon_app, "QRL TEST", ui_buffer, 32, 11},
#else
        {NULL, NULL, 0, &C_icon_app, "QRL", ui_buffer, 32, 11},
#endif
        {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
        {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
        UX_MENU_END
};

const ux_menu_entry_t menu_main_not_ready[] = {
#if TESTING_ENABLED
        {NULL, NULL, 0, &C_icon_app, "QRL TEST", ui_buffer, 32, 11},
#else
        {NULL, NULL, 0, &C_icon_app, "QRL", ui_buffer, 32, 11},
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

static const bagl_element_t bagl_ui_keygen[] = {
        UI_FillRectangle(0, 0, 0, 128, 32, 0x000000, 0xFFFFFF),
        UI_LabelLine(1, 0, 12, 128, 11, 0xFFFFFF, 0x000000, "Key generation"),
        UI_LabelLine(2, 0, 25, 128, 11, 0xFFFFFF, 0x000000, ui_buffer),
};

static const bagl_element_t bagl_ui_info_valuescrolling[] = {
    UI_FillRectangle(0, 0, 0, 128, 32, 0x000000, 0xFFFFFF),
    UI_Icon(0, 0, 0, 7, 7, BAGL_GLYPH_ICON_LEFT),
    UI_Icon(0, 128 - 7, 0, 7, 7, BAGL_GLYPH_ICON_RIGHT),
//    UI_LabelLine(1, 0, 8, 128, 11, 0xFFFFFF, 0x000000, (const char *) pageInfo),
    UI_LabelLine(1, 0, 19, 128, 11, 0xFFFFFF, 0x000000, (const char *) infoDataKey),
    UI_LabelLineScrolling(2, 0, 30, 128, 11, 0xFFFFFF, 0x000000, (const char *) infoDataValue),
};

void io_seproxyhal_display(const bagl_element_t* element)
{
    io_seproxyhal_display_default((bagl_element_t*) element);
}

const bagl_element_t* menu_main_prepro(const ux_menu_entry_t* menu_entry, bagl_element_t* element)
{
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

static unsigned int bagl_ui_keygen_button(
        unsigned int button_mask,
        unsigned int button_mask_counter)
{
    switch (button_mask) { }
    return 0;
}

////////////////////////////////////////////////
////////////////////////////////////////////////

void view_init(void)
{
    UX_INIT();
    view_uiState = UI_IDLE;
}

void view_idle(void)
{
    view_uiState = UI_IDLE;

    if (N_appdata.mode!=APPMODE_READY) {
        UX_MENU_DISPLAY(0, menu_main_not_ready, menu_main_prepro);
    }
    else {
        UX_MENU_DISPLAY(0, menu_main, menu_main_prepro);
    }
}

void view_sign(void)
{
    view_uiState = UI_SIGN;
    UX_MENU_DISPLAY(0, menu_sign, NULL);
}

void view_update_state(uint16_t interval)
{
    switch (N_appdata.mode) {
    case APPMODE_NOT_INITIALIZED: {
        snprintf(ui_buffer, sizeof(ui_buffer), "not ready ");
    }
        break;
    case APPMODE_KEYGEN_RUNNING: {
        snprintf(ui_buffer, sizeof(ui_buffer), "KEYGEN rem:%03d", 256-N_appdata.xmss_index);
    }
        break;
    case APPMODE_READY: {
        if (N_appdata.xmss_index>250) {
            snprintf(ui_buffer, sizeof(ui_buffer), "WARN! rem:%03d", 256-N_appdata.xmss_index);
        }
        else {
            snprintf(ui_buffer, sizeof(ui_buffer), "READY rem:%03d", 256-N_appdata.xmss_index);
        }
    }
        break;
    }
    UX_CALLBACK_SET_INTERVAL(interval);
}

void handler_init_device(unsigned int unused)
{
    UNUSED(unused);
//    UX_DISPLAY(bagl_ui_keygen, NULL);

    while (keygen()) {
        view_update_state(50);
    }

    view_update_state(50);
}

void handler_view_tx(unsigned int unused)
{
    UNUSED(unused);
    debug_printf("Not implemented");
    view_update_state(2000);
}

void handler_sign_tx(unsigned int unused)
{
    UNUSED(unused);
    app_sign();
    view_update_state(2000);
}

void handler_reject_tx(unsigned int unused)
{
    UNUSED(unused);
    set_code(G_io_apdu_buffer, 0, APDU_CODE_COMMAND_NOT_ALLOWED);
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    view_idle();
}
