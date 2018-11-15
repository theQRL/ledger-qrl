// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include "xmss_types.h"

#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct {
  xmss_sk_t sk;
  xmss_signature_t signature;
  uint8_t wots_buffer[WOTS_LEN * WOTS_N];
  uint8_t xmss_nodes[XMSS_NODES_BUFSIZE];
} N_DATA_t;

extern NVCONST N_DATA_t N_DATA_impl;

#ifdef LEDGER_SPECIFIC
#define N_DATA (*(N_DATA_t *)PIC(&N_DATA_impl))
#else
#define N_DATA N_DATA_impl
#endif

#ifdef  __cplusplus
}
#endif
