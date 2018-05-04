#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  7u

#define INS_VERSION             0u
#define INS_PUBLIC_KEY          1u

#define INS_TEST_PK_GEN_1       100u
#define INS_TEST_PK_GEN_2       101u
#define INS_TEST_PK             102u

#define INS_TEST_WRITE_LEAF     103u    // This command allows overwriting internal xmss leaves
#define INS_TEST_READ_LEAF      104u    // This command allows reading internal xmss leaves
#define INS_TEST_SIGN           105u

void app_init();

void app_main();