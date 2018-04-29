// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <stdint.h>
#include "macros.h"
#include "parameters.h"
#include "shash.h"
#include "adrs.h"

__INLINE void BE_inc(uint32_t* val) { *val = NtoHL(HtoNL(*val)+1); }

__INLINE void wotsp_expand_seed(uint8_t* pk, const uint8_t* seed)
{
    union shash_input_t prf_input;
    PRF_init(&prf_input, SHASH_TYPE_PRF);

    memcpy(prf_input.key, seed, WOTS_N);
    for (; prf_input.seed_gen.cdr<WOTS_LEN;
           prf_input.seed_gen.cdr++, pk += WOTS_N) {
        shash(pk, &prf_input);
    }
}

__INLINE void hash_f(uint8_t* in_out, union shash_input_t* shash_in)
{
    union shash_input_t h_in;
    PRF_init(&h_in, SHASH_TYPE_F);

    shash_in->adrs.otshash.keyAndMask = 0;
    shash(h_in.key, shash_in);

    shash_in->adrs.otshash.keyAndMask = HtoNL(1u);
    shash(h_in.F.mask, shash_in);

    for (uint8_t k = 0; k<WOTS_N; k++) {
        h_in.F.mask[k] ^= in_out[k];
    }

    shash(in_out, &h_in);
}

__INLINE void wotsp_gen_chain(uint8_t* in_out, union shash_input_t* prf_input, uint8_t start, int8_t count)
{
    prf_input->adrs.otshash.hash = HtoNL(start);
    for (int8_t i = start; i<start+count && i<WOTS_W; i++) {
        hash_f(in_out, prf_input);
        BE_inc(&prf_input->adrs.otshash.hash);
    }
}

__INLINE void wotsp_gen_pk(uint8_t* pk, uint8_t* sk, uint8_t index)
{
    wotsp_expand_seed(pk, sk);

    union shash_input_t prf_input;
    PRF_init(&prf_input, SHASH_TYPE_PRF);

    // TODO: pub_seed
    uint8_t pub_seed[32];
    memset(pub_seed, 0, 32);
    memcpy(prf_input.key, pub_seed, WOTS_N);

    ADRS_init(&prf_input.adrs, 0);
    prf_input.adrs.otshash.OTS = HtoNL(index);
    while (NtoHL(prf_input.adrs.otshash.chain)<WOTS_LEN) {
        wotsp_gen_chain(pk, &prf_input, 0, WOTS_W-1);
        BE_inc(&prf_input.adrs.otshash.chain);
        pk += WOTS_N;
    }
}

__INLINE void wotsp_sign(uint8_t* out_sig, const uint8_t* msg, const uint8_t* sk, uint8_t index)
{
    wotsp_expand_seed(out_sig, sk);

    union shash_input_t prf_input;
    PRF_init(&prf_input, SHASH_TYPE_PRF);
    // TODO: pub_seed
    uint8_t pub_seed[32];
    memset(pub_seed, 0, 32);
    memcpy(prf_input.key, pub_seed, WOTS_N);

    ///////////////

    {
        uint32_t csum = 0;
        uint32_t total = 0;
        uint32_t in = 0;
        uint8_t bits = 0;

        while (NtoHL(prf_input.adrs.otshash.chain)<WOTS_LEN) {
            if (bits==0) {
                bits += 8;
                if (HtoNL(prf_input.adrs.otshash.chain)<WOTS_LEN1) {
                    total = msg[in++];
                }
                else {
                    total = csum;
                    bits += 4;
                }
            }

            bits -= 4;
            const auto basew_i = (const uint8_t)((total >> bits) & 0x0Fu);
            wotsp_gen_chain(out_sig, &prf_input, 0, basew_i);

            BE_inc(&prf_input.adrs.otshash.chain);
            out_sig += WOTS_N;
            csum += (0x0Fu-basew_i);
        }
    }
}
