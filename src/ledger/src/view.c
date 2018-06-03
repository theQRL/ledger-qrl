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
#include "view.h"

ux_state_t ux;
enum UI_STATE view_uiState;

////////////////////////////////////////////////
//------ View elements

const ux_menu_entry_t menu_main[];
const ux_menu_entry_t menu_about[];

const ux_menu_entry_t menu_main[] = {
        {NULL, NULL, 0, &C_icon_app, "Example", "Demo", 32, 11},
        {menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
        {NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
        UX_MENU_END
};

const ux_menu_entry_t menu_about[] = {
        {menu_main, NULL, 0, &C_icon_back, "Version", APPVERSION, 0, 0},
        UX_MENU_END
};

void io_seproxyhal_display(const bagl_element_t *element) {
    io_seproxyhal_display_default((bagl_element_t *) element);
}

////////////////////////////////////////////////
//------ Event handlers

static const bagl_element_t *io_seproxyhal_touch_exit(const bagl_element_t *e) {
    os_sched_exit(0);   // Go back to the dashboard
    return NULL; // do not redraw the widget
}

static unsigned int bagl_ui_idle_nanos_button(unsigned int button_mask,
                                              unsigned int button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT:
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
            io_seproxyhal_touch_exit(NULL);
            break;
    }
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
    UX_MENU_DISPLAY(0, menu_main, NULL);
}
