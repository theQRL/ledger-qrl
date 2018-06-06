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

#define CLA                     0x77
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  0x07u

// TODO: Renumber all instructions

#define INS_VERSION             0x00u
#define INS_GETSTATE            0x01u
#define INS_PUBLIC_KEY          0x03u
#define INS_SIGN                0x04u
#define INS_SIGN_NEXT           0x05u

#define INS_TEST_PK_GEN_1       0x80
#define INS_TEST_PK_GEN_2       0x81
#define INS_TEST_CALC_PK        0x82
#define INS_TEST_WRITE_LEAF     0x83    // This command allows overwriting internal xmss leaves
#define INS_TEST_READ_LEAF      0x84    // This command allows reading internal xmss leaves
#define INS_TEST_KEYGEN         0x85
#define INS_TEST_DIGEST         0x86
#define INS_TEST_SETSTATE       0x87
#define INS_TEST_COMM           0x88

#define APPMODE_NOT_INITIALIZED    0x00
#define APPMODE_KEYGEN_RUNNING     0x01
#define APPMODE_READY              0x02
