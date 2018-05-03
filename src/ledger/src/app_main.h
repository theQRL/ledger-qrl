#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  7u

#define INS_VERSION             0u

#define INS_TEST_WOTS_PK_GEN    100u
#define INS_TEST_WOTS_SIGN      101u

void app_init();

void app_main();