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
#include "storage.h"
#include "view_templates.h"

ux_state_t ux;
enum UI_STATE view_uiState;
uint8_t _async_redisplay;
char ui_buffer[20];

////////////////////////////////////////////////
//------ View elements

const ux_menu_entry_t menu_main[];
const ux_menu_entry_t menu_about[];

const ux_menu_entry_t menu_main[] = {
#if TESTING_ENABLED
        {NULL, NULL, 0, &C_icon_app, "QRL TEST", ui_buffer, 32, 11},
#else
        {NULL, NULL, 0, &C_icon_app, "QRL", ui_buffer, 20, 0},
#endif
        {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
        {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
        UX_MENU_END
};

const ux_menu_entry_t menu_main_not_ready[] = {
#if TESTING_ENABLED
        {NULL, NULL, 0, &C_icon_app, "QRL TEST", ui_buffer, 32, 11},
#else
        {NULL, NULL, 0, &C_icon_app, "QRL", ui_buffer, 20, 0},
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

void io_seproxyhal_display(const bagl_element_t* element)
{
    io_seproxyhal_display_default((bagl_element_t*) element);
}

////////////////////////////////////////////////
//------ Event handlers

//static const bagl_element_t* io_seproxyhal_touch_exit(const bagl_element_t* e)
//{
//    os_sched_exit(0);   // Go back to the dashboard
//    return NULL; // do not redraw the widget
//}

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
        UX_MENU_DISPLAY(0, menu_main_not_ready, NULL);
    }
    else {
        UX_MENU_DISPLAY(0, menu_main, NULL);
    }
}

void view_update_state(uint16_t interval)
{
    switch (N_appdata.mode) {
    case APPMODE_NOT_INITIALIZED: {
        memcpy(ui_buffer, "not ready", 10);
    }
        break;
    case APPMODE_KEYGEN_RUNNING: {
        snprintf(ui_buffer, sizeof(ui_buffer), "keygen %03d/256", N_appdata.xmss_index);
    }
        break;
    case APPMODE_READY: {
        if (N_appdata.xmss_index>250) {
            snprintf(ui_buffer, sizeof(ui_buffer), "WARNING %03d/256", N_appdata.xmss_index+1);
        }
        else {
            snprintf(ui_buffer, sizeof(ui_buffer), "READY %03d/256", N_appdata.xmss_index+1);
        }
    }
        break;
    }
    _async_redisplay = 1;
    UX_CALLBACK_SET_INTERVAL(interval);
}

void handler_init_device(unsigned int unused)
{
    UNUSED(unused);

//    UX_DISPLAY(bagl_ui_keygen, NULL);

    while(keygen())
    {
        view_update_state(50);
    }

    view_update_state(50);
}
