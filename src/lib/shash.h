// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <stdint.h>
#ifndef LEDGER_SPECIFIC
    #include <cstdio>
#endif
#include "macros.h"
#include "adrs.h"

#pragma pack(push, 1)
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

union hashh_t {
  struct {
    union shash_input_t basic;
    uint8_t bitmask1[32];
    uint8_t bitmask2[32];
  };

  struct {
    uint8_t _padding[32];
    union shash_input_t basic;
    uint8_t _tmp[32];
  } shift;

  struct {
    uint8_t _padding[32];
    uint8_t shifted_raw[128];
  };

  uint8_t raw[160];
};
#pragma pack(pop)

#define SHASH_TYPE_F         0u
#define SHASH_TYPE_H         1u
#define SHASH_TYPE_HASH      2u
#define SHASH_TYPE_PRF       3u

#ifndef LEDGER_SPECIFIC
__INLINE void dump_hex(const char *prefix, uint8_t* data, uint16_t size)
{
    printf("%s %04d ", prefix, size);
    for (int i = 0; i<size; i++) {
        if (i%32==0) printf("|");
        printf("%02X", *(data+i));
    }
    printf("\n");
}
#endif

__INLINE void PRF_init(union shash_input_t* shash_in, uint8_t type)
{
    memset(shash_in->raw, 0, 96);
    shash_in->type[31] = type;
}

__INLINE void memxor(uint8_t* in_out, const uint8_t* in, const uint8_t count)
{
    for (size_t i = 0; i<count; i++) {
        *(in_out+i) ^= *(in+i);
    }
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
#ifdef HASH_DEBUG
    dump_hex(in->raw, 96);
#endif

    SHA256(in->raw, 96, out);

#ifdef HASH_DEBUG
    dump_hex(out, 32);
#endif
}

__INLINE void shash2(uint8_t* out, union hashh_t* in)
{
#ifdef HASH_DEBUG
    dump_hex(in->shifted_raw, 128);
#endif

    SHA256(in->shifted_raw, 128, out);

#ifdef HASH_DEBUG
    dump_hex(out, 32);
#endif
}

#endif
