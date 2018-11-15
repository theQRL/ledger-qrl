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
#include "xmss_types.h"

__INLINE void xmss_pk(xmss_pk_t *pk_out, const xmss_sk_t *sk_in) {
    memcpy(pk_out->root, sk_in->root, 32);
    memcpy(pk_out->pub_seed, sk_in->pub_seed, 32);
}

void xmss_ltree_gen(NVCONST uint8_t *leaf, NVCONST uint8_t *tmp_wotspk, const uint8_t *pub_seed, uint16_t index);

void xmss_treehash(
    uint8_t *root_out,
    uint8_t *authpath,
    const uint8_t *nodes,
    const uint8_t *pub_seed,
    uint16_t leaf_index);

void xmss_randombits(NVCONST uint8_t *random_bits, const uint8_t sk_seed[48]);

void xmss_get_seed_i(uint8_t *seed, const xmss_sk_t *sk, uint16_t idx);

void xmss_gen_keys_1_get_seeds(
    NVCONST xmss_sk_t *sk,
    const uint8_t *sk_seed);

void xmss_gen_keys_2_get_nodes(
    NVCONST uint8_t *wots_buffer,
    NVCONST uint8_t *xmss_node,
    const xmss_sk_t *sk, uint16_t idx);

void xmss_gen_keys_3_get_root(const uint8_t *xmss_nodes, NVCONST xmss_sk_t *sk);

void xmss_gen_keys(xmss_sk_t *sk, const uint8_t *sk_seed);

void xmss_digest(xmss_digest_t *digest, const uint8_t msg[32], const xmss_sk_t *sk, uint16_t index);

void xmss_sign(
    xmss_signature_t *sig,
    const uint8_t msg[32],
    const xmss_sk_t *sk,
    const uint8_t xmss_nodes[XMSS_NODES_BUFSIZE],
    uint16_t index);

void xmss_sign_incremental_init(
    xmss_sig_ctx_t *ctx,
    const uint8_t msg[32],
    const xmss_sk_t *sk,
    uint8_t xmss_nodes[XMSS_NODES_BUFSIZE],
    uint16_t index);

bool xmss_sign_incremental(
    xmss_sig_ctx_t *ctx,
    uint8_t *out,
    const xmss_sk_t *sk,
    uint16_t index);

bool xmss_sign_incremental_last(
    xmss_sig_ctx_t *ctx,
    uint8_t *out,
    const xmss_sk_t *sk,
    uint16_t index);
