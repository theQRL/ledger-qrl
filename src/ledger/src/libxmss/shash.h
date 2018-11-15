// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once

#include <stdint.h>
#include "zxmacros.h"
#include "adrs.h"
#include "parameters.h"

#pragma pack(push, 1)
typedef union {
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
    uint8_t _padding[92];
    uint32_t index;
  } R;

  struct {
    uint8_t type[32];
    uint8_t key[32];
    uint8_t _padding[31];
    uint8_t cdr;
  } seed_gen;
} shash_input_t;

typedef union {
  struct {
    shash_input_t basic;
    uint8_t bitmask1[32];
    uint8_t bitmask2[32];
  };

  struct {
    uint8_t _padding[32];
    shash_input_t basic;
    uint8_t _tmp[32];
  } shift;

  struct {
    uint8_t _padding[32];
    uint8_t shifted_raw[128];
  };

  struct {
    uint8_t type[32];
    uint8_t R[32];
    uint8_t root[32];
    uint8_t _padding[28];
    uint32_t index;
    uint8_t msg_hash[32];
  } digest;

  uint8_t raw[160];
} hashh_t;
#pragma pack(pop)

#define SHASH_TYPE_F         0u
#define SHASH_TYPE_H         1u
#define SHASH_TYPE_HASH      2u
#define SHASH_TYPE_PRF       3u

#ifndef LEDGER_SPECIFIC
#include <stdio.h>
__INLINE void dump_hex(const char *prefix, uint8_t *data, uint16_t size) {
    printf("%s %04d ", prefix, size);
    for (int i = 0; i < size; i++) {
        if (i % 32 == 0)
            printf("|");
        printf("%02X", *(data + i));
    }
    printf("\n");
}
#endif

__INLINE void PRF_init(shash_input_t *shash_in, uint8_t type) {
    memset(shash_in->raw, 0, 96);
    shash_in->type[31] = type;
}

__INLINE void memxor(uint8_t *in_out, const uint8_t *in, const uint8_t count) {
    for (size_t i = 0; i < count; i++) {
        *(in_out + i) ^= *(in + i);
    }
}

#ifdef LEDGER_SPECIFIC
#include "os.h"
#include "cx.h"
__INLINE void __sha256(uint8_t *out, const uint8_t* in, uint16_t in_len)
{
    cx_hash_sha256(in, in_len, out, 32);
}

#else

#include <openssl/sha.h>
__INLINE void __sha256(uint8_t *out, const uint8_t *in, uint16_t in_len) {
    SHA256(in, in_len, out);
}

#endif

__INLINE void shash96(uint8_t *out, const shash_input_t *in) {
    __sha256(out, in->raw, 96);
}

__INLINE void shash128_shifted(uint8_t *out, const hashh_t *in) {
    __sha256(out, in->shifted_raw, 128);
}

__INLINE void shash160(uint8_t *out, const hashh_t *in) {
    __sha256(out, in->raw, 160);
}

__INLINE void hash_f(uint8_t *in_out, shash_input_t *shash_in) {
    shash_input_t h_in;
    PRF_init(&h_in, SHASH_TYPE_F);

    shash_in->adrs.keyAndMask = 0;
    shash96(h_in.key, shash_in);

    shash_in->adrs.keyAndMask = HtoNL(1u);
    shash96(h_in.F.mask, shash_in);

    memxor(h_in.F.mask, in_out, WOTS_N);

    shash96(in_out, &h_in);
}

__INLINE void shash_h(uint8_t *out, const uint8_t *in, hashh_t *hhash_in) {
    hhash_in->basic.type[31] = SHASH_TYPE_PRF;

    hhash_in->basic.adrs.keyAndMask = HtoNL(1u);
    shash96(hhash_in->bitmask1, &hhash_in->basic);

    hhash_in->basic.adrs.keyAndMask = HtoNL(2u);
    shash96(hhash_in->bitmask2, &hhash_in->basic);

    // Shifting hhash
    hhash_in->basic.adrs.keyAndMask = HtoNL(0u);
    shash96(hhash_in->shift.basic.key, &hhash_in->basic);

    memset(hhash_in->shift.basic.type, 0, WOTS_N);
    hhash_in->shift.basic.type[31] = SHASH_TYPE_H;

    memxor(hhash_in->bitmask1, in, WOTS_N);
    memxor(hhash_in->bitmask2, in + WOTS_N, WOTS_N);

    shash128_shifted(out, hhash_in);
}
