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
#pragma once

#include "os.h"
#include "cx.h"
#include <os_io_seproxyhal.h>

#define MAX_CHARS_PER_KEY_LINE      32
#define MAX_CHARS_PER_VALUE_LINE    128

extern char view_buffer_key[MAX_CHARS_PER_KEY_LINE];
extern char view_buffer_value[MAX_CHARS_PER_VALUE_LINE];

enum UI_STATE {
  UI_IDLE,
  UI_SIGN
};

extern enum UI_STATE view_uiState;

void view_init(void);
void view_main_menu(void);
void view_sign_menu(void);
void view_update_state(uint16_t interval);

void handler_view_tx(unsigned int unused);
void handler_sign_tx(unsigned int unused);
void handler_reject_tx(unsigned int unused);
