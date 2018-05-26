// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <xmss.h>

#define BUF_MAX_IDX 34      // split point between ram and nvram

__INLINE uint8_t* get_p(NVCONST uint8_t* tmp_wotspk, uint8_t *mem_wotspk, uint32_t idx)
{
    uint8_t *base_p = idx<BUF_MAX_IDX ? (uint8_t*)mem_wotspk : (uint8_t*)tmp_wotspk;
    return base_p + WOTS_N * idx;
}

void xmss_ltree_gen(NVCONST uint8_t* leaf, NVCONST uint8_t* tmp_wotspk, const uint8_t* pub_seed, uint16_t index)
{
    uint8_t mem_wotspk[BUF_MAX_IDX*WOTS_N];
    memcpy(mem_wotspk, tmp_wotspk, BUF_MAX_IDX*WOTS_N);

    // WARNING: This functions collapses wotspk and will be destroyed after the call
    uint8_t l = WOTS_LEN;
    uint8_t tree_height = 0;

    while (l>1) {
        const uint8_t bound = l >> 1u;

        for (uint8_t i = 0; i<bound; i++) {
            hashh_t hashh_in;
            memset(hashh_in.basic.raw, 0, 96);
            memcpy(hashh_in.basic.key, pub_seed, 32);

            hashh_in.basic.adrs.type = HtoNL(SHASH_TYPE_H);
            hashh_in.basic.adrs.trees.ltree = HtoNL(index);
            hashh_in.basic.adrs.trees.height = HtoNL(tree_height);
            hashh_in.basic.adrs.trees.index = HtoNL(i);

            uint8_t* src = get_p(tmp_wotspk, mem_wotspk, i*2u);
            uint8_t* dst = get_p(tmp_wotspk, mem_wotspk, i);
            shash_h(dst, src, &hashh_in);
        }

        if (l & 1u) {
            uint8_t *src = get_p(tmp_wotspk, mem_wotspk, (l-1u));
            uint8_t *dst = get_p(tmp_wotspk, mem_wotspk, (l >> 1u));
            l = (uint8_t) ((l >> 1u)+1u);

            memcpy(dst, src, WOTS_N);
        }
        else {
            l = (l >> 1u);
        }

        tree_height++;
    }

    nvcpy(leaf, mem_wotspk, WOTS_N);
}

void xmss_treehash(
        uint8_t* root_out,
        uint8_t* authpath,
        const uint8_t* nodes,
        const uint8_t* pub_seed,
        const uint16_t leaf_index)
{
    hashh_t h_in;
    uint8_t stack[XMSS_STK_SIZE];
    uint16_t stack_levels[XMSS_STK_LEVELS];
    uint32_t stack_offset = 0;

    for (uint16_t idx = 0; idx<XMSS_NUM_NODES; idx++) {
        // bring node in
        memcpy(stack+stack_offset*WOTS_N, nodes+idx*WOTS_N, WOTS_N);

        stack_levels[stack_offset] = 0;
        stack_offset++;
        if ((leaf_index ^ 0x1u)==idx) {
            memcpy(authpath, stack+(stack_offset-1)*WOTS_N, WOTS_N);
        }

        while (stack_offset>1 && stack_levels[stack_offset-1]==stack_levels[stack_offset-2]) {
            uint16_t tree_idx = (idx >> (stack_levels[stack_offset-1u]+1u));
            memset(h_in.raw, 0, 96);

            h_in.basic.adrs.type = HtoNL(SHASH_TYPE_HASH);
            h_in.basic.adrs.trees.height = HtoNL(stack_levels[stack_offset-1u]);
            h_in.basic.adrs.trees.index = HtoNL(tree_idx);

            memcpy(h_in.basic.key, pub_seed, WOTS_N);

            unsigned char* in_out = stack+(stack_offset-2)*WOTS_N;

            shash_h(in_out, in_out, &h_in);

            stack_levels[stack_offset-2]++;
            stack_offset--;

            if (((leaf_index >> stack_levels[stack_offset-1u]) ^ 0x1)==tree_idx) {
                memcpy(authpath+stack_levels[stack_offset-1]*WOTS_N, stack+(stack_offset-1)*WOTS_N, WOTS_N);
            }
        }
    }

    memcpy(root_out, stack, WOTS_N);
}

void xmss_randombits(NVCONST uint8_t* random_bits, const uint8_t sk_seed[48])
{
#ifdef LEDGER_SPECIFIC
    uint8_t buffer[3*WOTS_N];
        cx_sha3_t hash_sha3;
        cx_sha3_xof_init(&hash_sha3,256,3*WOTS_N);
        cx_hash(&hash_sha3.header, CX_LAST, sk_seed, 48, buffer, 3*WOTS_N);
        nvcpy(random_bits, buffer, 3*WOTS_N);
#else
    shake256(random_bits, 3*WOTS_N, sk_seed, 48);
#endif
}

void xmss_get_seed_i(uint8_t* seed, const xmss_sk_t* sk, uint16_t idx)
{
    shash_input_t prf_in;
    PRF_init(&prf_in, SHASH_TYPE_PRF);
    memcpy(prf_in.key, sk->seed, WOTS_N);
    prf_in.adrs.otshash.OTS = HtoNL(idx);
    shash96(seed, &prf_in);
}

void xmss_gen_keys_1_get_seeds(NVCONST xmss_sk_t* sk, const uint8_t* sk_seed)
{
    nvset(&sk->index, 0);
    xmss_randombits(sk->seeds.raw, sk_seed);
}

void xmss_gen_keys_2_get_nodes(
        NVCONST uint8_t* wots_buffer,
        NVCONST uint8_t* xmss_node,
        const xmss_sk_t* sk,
        uint16_t idx)
{
    uint8_t seed[WOTS_N];
    xmss_get_seed_i(seed, sk, idx);
    wotsp_gen_pk(wots_buffer, seed, sk->pub_seed, idx);
    xmss_ltree_gen(xmss_node, wots_buffer, sk->pub_seed, idx);
}

void xmss_gen_keys_3_get_root(const uint8_t* xmss_nodes, NVCONST xmss_sk_t* sk)
{
    uint8_t root[WOTS_N];
    uint8_t authpath[(XMSS_H+1)*WOTS_N];
    xmss_treehash(root, authpath, xmss_nodes, sk->pub_seed, 0);
    nvcpy(sk->root, root, WOTS_N);
}

void xmss_gen_keys(xmss_sk_t* sk, const uint8_t* sk_seed)
{
    xmss_gen_keys_1_get_seeds(sk, sk_seed);

    uint8_t xmss_nodes[XMSS_NODES_BUFSIZE];
    for (uint16_t idx = 0; idx<XMSS_NUM_NODES; idx++) {
        uint8_t wots_buffer[WOTS_LEN*WOTS_N];

        xmss_gen_keys_2_get_nodes(
                wots_buffer,
                xmss_nodes+idx*WOTS_N,
                sk, idx);
    }

    xmss_gen_keys_3_get_root(xmss_nodes, sk);
}

void xmss_digest(
        xmss_digest_t* digest,
        const uint8_t msg[32],
        const xmss_sk_t* sk,
        const uint16_t index)
{
    // get randomness
    shash_input_t prf_in;
    PRF_init(&prf_in, SHASH_TYPE_PRF);
    memcpy(prf_in.key, sk->prf_seed, WOTS_N);
    prf_in.R.index = HtoNL(index);
    shash96(digest->randomness, &prf_in);

    // Digest hash
    hashh_t h_in;
    memset(h_in.raw, 0, 160);
    h_in.digest.type[31] = SHASH_TYPE_HASH;
    memcpy(h_in.digest.R, digest->randomness, WOTS_N);
    memcpy(h_in.digest.root, sk->root, 32);
    h_in.digest.index = NtoHL(index);
    memcpy(h_in.digest.msg_hash, msg, 32);
    shash160(digest->hash, &h_in);
}

void xmss_sign(
        xmss_signature_t* sig,
        const uint8_t msg[32],
        const xmss_sk_t* sk,
        const uint8_t xmss_nodes[XMSS_NODES_BUFSIZE],
        const uint16_t index)
{
    // Get message digest
    xmss_digest_t msg_digest;
    xmss_digest(&msg_digest, msg, sk, index);

    // Signature xmss_signature_t
    sig->index = NtoHL(index);
    memcpy(sig->randomness, msg_digest.randomness, 32);

    // The following is a trick to reuse and save RAM
    uint8_t dummy_root[32];
    xmss_treehash(
            dummy_root,
            sig->auth_path,
            xmss_nodes,
            sk->pub_seed,
            index);

    // The following is a trick to reuse and save RAM
    uint8_t seed_i[32];
    xmss_get_seed_i(seed_i, sk, index);

    wotsp_sign(sig->wots_sig,
            msg_digest.hash,
            sk->pub_seed,
            seed_i,
            index);
}

void xmss_sign_incremental_init(
        xmss_sig_ctx_t* ctx,
        const uint8_t msg[32],
        const xmss_sk_t* sk,
        uint8_t xmss_nodes[XMSS_NODES_BUFSIZE],
        const uint16_t index)
{
    ctx->sig_chunk_idx = 0;
    ctx->written = 0;
    xmss_digest(&ctx->msg_digest, msg, sk, index);
    ctx->xmss_nodes=xmss_nodes;

    uint8_t seed_i[32];
    xmss_get_seed_i(seed_i, sk, index);

    wotsp_sign_init_ctx(
            &ctx->wots_ctx,
            sk->pub_seed,
            seed_i,
            index);
}

bool xmss_sign_incremental(
        xmss_sig_ctx_t* ctx,
        uint8_t* out,
        const xmss_sk_t* sk,
        const uint16_t index)
{
    ctx->written = 0;

    if (ctx->sig_chunk_idx>9)
    {
        return true;
    }

    // Incremental signature must be divided in 11 chunks
    //  4 + 32 ( 1 + 4 )             164     C=0
    //      32   7              N=9  224     C=1..9
    //      32   7                   224     C=10
    // Fill the buffer according to this structure
    // and return true when the signature is complete

    uint8_t wots_steps = 7;

    if (ctx->sig_chunk_idx==0) {
        // first block is different
        uint32_t* signature_index = (uint32_t*) out;
        *signature_index = NtoHL(index);
        ctx->written += 4;

        memcpy(out+ctx->written, ctx->msg_digest.randomness, 32);
        ctx->written += 32;

        wots_steps = 4;
    }

    // Normal steps add 7 wots steps
    for (int i = 0; i<wots_steps; i++) {
        wotsp_sign_step(&ctx->wots_ctx, out+ctx->written, ctx->msg_digest.hash);
        ctx->written += 32;
    }

    ctx->sig_chunk_idx++;
    return false;
}

bool xmss_sign_incremental_last(
        xmss_sig_ctx_t* ctx,
        uint8_t* out,
        const xmss_sk_t* sk,
        const uint16_t index)
{
    ctx->written = 0;

    if (ctx->sig_chunk_idx!=10) {
        return false;
    }

    // Last block is the authpath
    uint8_t dummy_root[32];
    xmss_treehash(
            dummy_root,
            out,
            ctx->xmss_nodes,
            sk->pub_seed,
            index);
    ctx->written += 7*32;
    ctx->sig_chunk_idx++;
    return true;
}
