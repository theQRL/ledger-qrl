#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  7u

#define INS_VERSION             0u
#define INS_PUBLIC_KEY          1u

#define INS_TEST_PK_GEN_1       100u
#define INS_TEST_PK_GEN_2       101u
#define INS_TEST_PK_GEN_3       102u

void app_init();

void app_main();