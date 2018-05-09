#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  0x07u

#define INS_VERSION             0x00u
#define INS_PUBLIC_KEY          0x01u

#define INS_TEST_PK_GEN_1       0x80
#define INS_TEST_PK_GEN_2       0x81
#define INS_TEST_PK             0x82

#define INS_TEST_WRITE_LEAF     0x83    // This command allows overwriting internal xmss leaves
#define INS_TEST_READ_LEAF      0x84    // This command allows reading internal xmss leaves
#define INS_TEST_DIGEST         0x85
#define INS_TEST_SIGN           0x86
#define INS_TEST_GET_SIGN       0x87

void app_init();

void app_main();