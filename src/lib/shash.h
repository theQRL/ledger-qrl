// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <stdint.h>
#include "macros.h"
#include "adrs.h"

union shash_input_t {
  uint8_t raw[96];

  struct {
    uint8_t type[32];
    uint8_t key[32];
    union ADRS_t adrs;
  };

  struct {
    uint8_t type[32];
    uint8_t key[32];
    uint8_t mask[32];
  } F;

  struct {
    uint8_t type[32];
    uint8_t key[32];
    uint8_t _padding[31];
    uint8_t cdr;
  } seed_gen;
};

#define SHASH_TYPE_F         0u
#define SHASH_TYPE_H         1u
#define SHASH_TYPE_HASH      2u
#define SHASH_TYPE_PRF       3u

__INLINE void PRF_init(union shash_input_t *shash_in, uint8_t type)
{
    memset(shash_in->raw, 0, 96);
    shash_in->type[31] = type;
}

#ifdef LEDGER_SPECIFIC
#include "os.h"
#include "cx.h"
__INLINE void shash(uint8_t *out, union shash_input_t* in)
{
    cx_hash_sha256(in->raw, 96, out, 32);
}
#else

#include <openssl/sha.h>

__INLINE void shash(uint8_t* out, union shash_input_t* in)
{
    SHA256(in->raw, 96, out);
}

#endif
