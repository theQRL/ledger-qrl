// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <stdint.h>
#include <zxmacros.h>
#include "parameters.h"
#include "shash.h"
#include "adrs.h"

typedef union {
  struct {
    uint32_t csum;
    uint32_t total;
    uint32_t in;
    uint8_t bits;
    shash_input_t prf_input;
  };
  uint8_t raw[13];
} wots_sign_ctx_t;

__INLINE void BE_inc(uint32_t* val) { *val = NtoHL(HtoNL(*val)+1); }

__INLINE void wotsp_expand_seed(uint8_t* pk, const uint8_t* seed)
{
    shash_input_t prf_input;
    PRF_init(&prf_input, SHASH_TYPE_PRF);

    memcpy(prf_input.key, seed, WOTS_N);
    for (; prf_input.seed_gen.cdr<WOTS_LEN;
           prf_input.seed_gen.cdr++, pk += WOTS_N) {
        shash96(pk, &prf_input);
    }
}

__INLINE void wotsp_gen_chain(uint8_t* in_out, shash_input_t* prf_input, uint8_t start, int8_t count)
{
    prf_input->adrs.otshash.hash = HtoNL(start);
    for (uint8_t i = start; i<start+count && i<WOTS_W; i++) {
        hash_f(in_out, prf_input);
        BE_inc(&prf_input->adrs.otshash.hash);
    }
}

__INLINE void wotsp_gen_pk(uint8_t* pk, uint8_t* sk, const uint8_t* pub_seed, uint16_t index)
{
    wotsp_expand_seed(pk, sk);

    shash_input_t prf_input;
    PRF_init(&prf_input, SHASH_TYPE_PRF);
    memcpy(prf_input.key, pub_seed, WOTS_N);

    ADRS_init(&prf_input.adrs, 0);
    prf_input.adrs.otshash.OTS = HtoNL(index);

    while (NtoHL(prf_input.adrs.otshash.chain)<WOTS_LEN) {
        wotsp_gen_chain(pk, &prf_input, 0, WOTS_W-1);
        BE_inc(&prf_input.adrs.otshash.chain);
        pk += WOTS_N;
    }
}

__INLINE void wotsp_sign_s1(
        uint8_t* out_sig,
        const uint8_t* sk)
{
    wotsp_expand_seed(out_sig, sk);
}

__INLINE void wotsp_sign_s2(
        wots_sign_ctx_t* ctx,
        uint8_t* out_sig,
        const uint8_t* msg,
        const uint8_t* pub_seed,
        uint16_t index)
{
    PRF_init(&ctx->prf_input, SHASH_TYPE_PRF);
    ctx->prf_input.adrs.otshash.OTS = NtoHL(index);
    memcpy(ctx->prf_input.key, pub_seed, WOTS_N);

    // init context
    ctx->bits = 0;
    ctx->csum = 0;
    ctx->in = 0;
    ctx->total = 0;
}

__INLINE void wotsp_sign_s3(
        wots_sign_ctx_t* ctx,
        uint8_t* out_sig_p,
        const uint8_t* msg)
{
    if (ctx->bits==0) {
        ctx->bits += 8;
        if (NtoHL(ctx->prf_input.adrs.otshash.chain)<WOTS_LEN1) {
            ctx->total = msg[ctx->in++];
        }
        else {
            ctx->total = ctx->csum;
            ctx->bits += 4;
        }
    }

    ctx->bits -= 4;
    const uint8_t basew_i = (uint8_t) ((ctx->total >> ctx->bits) & 0x0Fu);
    wotsp_gen_chain(out_sig_p, &ctx->prf_input, 0, basew_i);

    ctx->csum += (0x0Fu-basew_i);
    BE_inc(&ctx->prf_input.adrs.otshash.chain);
}

__INLINE void wotsp_sign(
        uint8_t* out_sig,
        const uint8_t* msg,
        const uint8_t* pub_seed,
        const uint8_t* sk,
        uint16_t index)
{
    wots_sign_ctx_t ctx;

    wotsp_sign_s1(out_sig, sk);
    wotsp_sign_s2(&ctx, out_sig, msg, pub_seed, index);

    uint16_t offset=0;
    while (NtoHL(ctx.prf_input.adrs.otshash.chain)<WOTS_LEN) {
        uint8_t *p = out_sig + offset;
        wotsp_sign_s3(&ctx, p, msg);
        offset+=WOTS_N;
    }
}
