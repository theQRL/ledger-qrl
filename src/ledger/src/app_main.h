#pragma once

#define CLA 0x80
#define OFFSET_CLA 0
#define OFFSET_INS 1

#define APP_ID                  0x07u

#define INS_VERSION             0x00u
#define INS_GETSTATE            0x01u
#define INS_KEYGEN              0x02u
#define INS_PUBLIC_KEY          0x03u
#define INS_SIGN                0x04u
#define INS_SIGN_NEXT           0x05u

#define INS_TEST_PK_GEN_1       0x80
#define INS_TEST_PK_GEN_2       0x81

#define INS_TEST_WRITE_LEAF     0x83    // This command allows overwriting internal xmss leaves
#define INS_TEST_READ_LEAF      0x84    // This command allows reading internal xmss leaves
#define INS_TEST_DIGEST         0x85
#define INS_TEST_SIGN_INIT      0x86
#define INS_TEST_SIGN_NEXT      0x87
#define INS_TEST_SETSTATE       0x88

#define APPMODE_NOT_INITIALIZED    0x00
#define APPMODE_KEYGEN_RUNNING     0x01
#define APPMODE_READY              0x02

#pragma pack(push, 1)
typedef union  {
  struct {
    uint8_t mode;
    uint16_t xmss_index;
  };
  uint8_t raw[3];
} N_APPSTATE_t;
#pragma pack(pop)

void app_init();
void app_main();