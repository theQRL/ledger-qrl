/*******************************************************************************
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
#include "xmss_types.h"

#pragma pack(push, 1)
typedef union {
  struct {
    uint8_t mode;
    uint16_t xmss_index;

    ////
    xmss_pk_t pk;
  };
  uint8_t raw[3];

} appstorage_t;
#pragma pack(pop)

extern appstorage_t N_appdata_impl;
#define N_appdata (*(appstorage_t *)PIC(&N_appdata_impl))
