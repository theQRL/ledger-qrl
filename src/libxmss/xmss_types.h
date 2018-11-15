// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <stdint.h>
#include "zxmacros.h"
#include "parameters.h"
#include "shash.h"
#include "adrs.h"
#include "fips202.h"
#include "wotsp.h"

#pragma pack(push, 1)
typedef union {
  uint8_t raw[132];
  struct {
    uint32_t index;
    uint8_t seed[32];
    uint8_t prf_seed[32];
    uint8_t pub_seed[32];
    uint8_t root[32];
  };
  struct {
    uint32_t _padding;
    uint8_t raw[96];
    uint8_t _padding2[32];
  } seeds;
} xmss_sk_t;

typedef union {
  uint8_t raw[64];
  struct {
    uint8_t root[32];
    uint8_t pub_seed[32];
  };
} xmss_pk_t;

typedef union {
  uint8_t raw[64];
  struct {
    uint8_t hash[32];
    uint8_t randomness[32];
  };
} xmss_digest_t;

typedef union {
  uint8_t raw[XMSS_SIGSIZE];
  struct {
    uint32_t index;
    uint8_t randomness[32];
    uint8_t wots_sig[32 * 67];
    uint8_t auth_path[32 * (XMSS_H - 1)];
  };
} xmss_signature_t;

typedef union {
  struct {
    uint16_t written;
    uint8_t sig_chunk_idx;
    xmss_digest_t msg_digest;
    wots_sign_ctx_t wots_ctx;
    uint8_t *xmss_nodes;
  };
} xmss_sig_ctx_t;
#pragma pack(pop)
