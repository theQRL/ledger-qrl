#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define INS_VERSION         0
#define INS_WOTS_GEN_PK     1

void app_init();

void app_main();