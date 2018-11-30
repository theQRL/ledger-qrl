// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <gmock/gmock.h>

#include <xmss-alt/wots.h>
#include <xmss-alt/wots_internal.h>
#include <xmss-alt/hash_address.h>
#include <xmss-alt/xmss_common.h>
#include <xmss-alt/algsxmss.h>
#include <xmss-alt/hash.h>
#include <qrl/xmssFast.h>

extern "C"
{
#include <parameters.h>
#include <wotsp.h>
#include <shash.h>
#include <adrs.h>
#include <xmss.h>
#include <nvram.h>
#include <xmss_types.h>
#include <qrl_types.h>
}

namespace {
    TEST(XMSS, hash_h_0) {
        std::vector<uint8_t> data(2 * WOTS_N);
        std::vector<uint8_t> out_ledger(WOTS_N);
        std::vector<uint8_t> out_qrllib(WOTS_N);

        std::vector<uint8_t> pub_seed(WOTS_N);

        hashh_t hashh_in{};
        memset(hashh_in.basic.raw, 0, 96);
        hashh_in.basic.adrs.type = HtoNL(SHASH_TYPE_H);
        hashh_in.basic.adrs.trees.ltree = HtoNL(0u);
        hashh_in.basic.adrs.trees.height = HtoNL(0u);
        hashh_in.basic.adrs.trees.index = HtoNL(0u);
        printf("\n");
        shash_h(out_ledger.data(), data.data(), &hashh_in);

        uint32_t ots_addr[8]{};
        setType(ots_addr, SHASH_TYPE_H);
        setLtreeADRS(ots_addr, 0);
        printf("\n");
        hash_h(eHashFunction::SHA2_256, out_qrllib.data(), data.data(), pub_seed.data(), ots_addr, WOTS_N);

        EXPECT_THAT(out_ledger, ::testing::Eq(out_qrllib));
    }

    TEST(XMSS, hash_h_1) {
        std::vector<uint8_t> data(2 * WOTS_N);
        std::vector<uint8_t> out_ledger(WOTS_N);
        std::vector<uint8_t> out_qrllib(WOTS_N);

        std::vector<uint8_t> pub_seed(WOTS_N);

        hashh_t hashh_in{};
        memset(hashh_in.basic.raw, 0, 96);
        hashh_in.basic.adrs.type = HtoNL(SHASH_TYPE_H);
        hashh_in.basic.adrs.trees.ltree = HtoNL(1u);
        hashh_in.basic.adrs.trees.height = HtoNL(0u);
        hashh_in.basic.adrs.trees.index = HtoNL(0u);
        printf("\n");
        shash_h(out_ledger.data(), data.data(), &hashh_in);

        uint32_t ots_addr[8]{};
        setType(ots_addr, SHASH_TYPE_H);
        setLtreeADRS(ots_addr, 1);
        printf("\n");
        hash_h(eHashFunction::SHA2_256, out_qrllib.data(), data.data(), pub_seed.data(), ots_addr, WOTS_N);

        EXPECT_THAT(out_ledger, ::testing::Eq(out_qrllib));
    }

    TEST(XMSS, ltree_gen_0) {
        std::vector<uint8_t> leaf_ledger(WOTS_N);
        std::vector<uint8_t> leaf_qrllib(WOTS_N);

        std::vector<uint8_t> pk_1(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> pk_2(WOTS_N * WOTS_LEN);

        std::vector<uint8_t> sk(WOTS_N);
        std::vector<uint8_t> pub_seed(WOTS_N);

        uint8_t wots_index = 0;
        wotsp_gen_pk(pk_1.data(), sk.data(), pub_seed.data(), wots_index);
        wotsp_gen_pk(pk_2.data(), sk.data(), pub_seed.data(), wots_index);

        xmss_ltree_gen(leaf_ledger.data(), pk_1.data(), pub_seed.data(), wots_index);

        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};
        setType(ots_addr, 1);
        setLtreeADRS(ots_addr, 0);

        l_tree(eHashFunction::SHA2_256,
               &params,
               leaf_qrllib.data(),
               pk_2.data(),
               pub_seed.data(),
               ots_addr);

        EXPECT_THAT(leaf_ledger, ::testing::Eq(leaf_qrllib));
    }

    TEST(XMSS, ltree_gen_1) {
        std::vector<uint8_t> leaf_ledger(WOTS_N);
        std::vector<uint8_t> leaf_qrllib(WOTS_N);

        std::vector<uint8_t> pk_1(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> pk_2(WOTS_N * WOTS_LEN);

        std::vector<uint8_t> sk(WOTS_N);
        std::vector<uint8_t> pub_seed(WOTS_N);
        for (uint8_t i = 0; i < 32u; i++)
            pub_seed[i] = i << 1u;

        uint8_t wots_index = 0;
        wotsp_gen_pk(pk_1.data(), sk.data(), pub_seed.data(), wots_index);
        wotsp_gen_pk(pk_2.data(), sk.data(), pub_seed.data(), wots_index);

        xmss_ltree_gen(leaf_ledger.data(), pk_1.data(), pub_seed.data(), wots_index);

        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};
        setType(ots_addr, 1);
        setLtreeADRS(ots_addr, 0);

        l_tree(eHashFunction::SHA2_256,
               &params,
               leaf_qrllib.data(),
               pk_2.data(),
               pub_seed.data(),
               ots_addr);

        EXPECT_THAT(leaf_ledger, ::testing::Eq(leaf_qrllib));
    }

    TEST(XMSS, gen_pk_phase_1) {
        std::vector<uint8_t> sk_seed(48);

        xmss_pk_t pk_1{};
        xmss_pk_t pk_2{};
        xmss_sk_t sk_1{};
        xmss_sk_t sk_2{};

        xmss_gen_keys_1_get_seeds(&sk_1, sk_seed.data());
        xmss_pk(&pk_1, &sk_1);

        xmss_params params{};
        xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
        xmss_Genkeypair(eHashFunction::SHA2_256,
                        &params,
                        pk_2.raw,
                        sk_2.raw,
                        sk_seed.data());

        dump_hex("", sk_1.seed, 32);
        dump_hex("", sk_1.prf_seed, 32);
        dump_hex("", sk_1.pub_seed, 32);

        for (int i = 0; i < 32; i++)
            ASSERT_EQ(pk_1.pub_seed[i], pk_2.pub_seed[i]);

        for (int i = 0; i < 32; i++)
            ASSERT_EQ(sk_1.pub_seed[i], sk_2.pub_seed[i]);

        for (int i = 0; i < 96; i++)
            ASSERT_EQ(sk_1.seeds.raw[i], sk_2.seeds.raw[i]);
    }

    TEST(XMSS, gen_pk_phase_2_zeroseed_many) {
        std::vector<uint8_t> sk_seed(48);
        xmss_sk_t sk_1{};
        uint8_t xmss_node_ledger[32];
        uint8_t xmss_node_qrllib[32];

        xmss_gen_keys_1_get_seeds(&sk_1, sk_seed.data());

        std::cout << std::endl;

        for (uint16_t idx = 0; idx < 256; idx += 1) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];
            xmss_gen_keys_2_get_nodes(wots_buffer, xmss_node_ledger, &sk_1, idx);

            ////
            xmss_params params{};
            xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);

            uint32_t ots_addr[8]{};
            uint32_t ltree_addr[8]{};
            setType(ltree_addr, 1);
            setLtreeADRS(ltree_addr, idx);
            setOTSADRS(ots_addr, idx);

            gen_leaf_wots(
                    eHashFunction::SHA2_256,
                    xmss_node_qrllib,
                    sk_1.seed,
                    &params,
                    sk_1.pub_seed,
                    ltree_addr,
                    ots_addr);

            dump_hex("", xmss_node_ledger, 32);

            for (int i = 0; i < 32; i++)
                ASSERT_EQ(xmss_node_ledger[i], xmss_node_qrllib[i]);
        }
    }

    TEST(XMSS, gen_pk_zeros) {
        std::vector<uint8_t> sk_seed(48);

        xmss_pk_t pk_1{};
        xmss_pk_t pk_2{};
        xmss_sk_t sk_1{};
        xmss_sk_t sk_2{};

        std::cout << std::endl;

        xmss_gen_keys(&sk_1, sk_seed.data());
        xmss_pk(&pk_1, &sk_1);

        xmss_params params{};
        xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
        xmss_Genkeypair(eHashFunction::SHA2_256,
                        &params,
                        pk_2.raw,
                        sk_2.raw,
                        sk_seed.data());

        dump_hex("Tx: ", pk_1.root, 64);
        dump_hex("Tx: ", pk_2.root, 64);

        for (int i = 0; i < 64; i++) {
            ASSERT_EQ(pk_1.raw[i], pk_2.raw[i]);
        }

        for (int i = 0; i < 132; i++) {
            ASSERT_EQ(sk_1.raw[i], sk_2.raw[i]);
        }
    }

    TEST(XMSS, gen_pk_somekey) {
        std::vector<uint8_t> sk_seed(48);

        for (uint8_t i = 0; i < 48u; i++)
            sk_seed[i] = i << 1u;

        xmss_pk_t pk_1{};
        xmss_pk_t pk_2{};
        xmss_sk_t sk_1{};
        xmss_sk_t sk_2{};

        std::cout << std::endl;

        xmss_gen_keys(&sk_1, sk_seed.data());
        xmss_pk(&pk_1, &sk_1);

        xmss_params params{};
        xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
        xmss_Genkeypair(eHashFunction::SHA2_256,
                        &params,
                        pk_2.raw,
                        sk_2.raw,
                        sk_seed.data());

        dump_hex("Tx: ", pk_1.root, 32);
        dump_hex("Tx: ", pk_2.root, 32);

        for (int i = 0; i < 64; i++) {
            ASSERT_EQ(pk_1.raw[i], pk_2.raw[i]);
        }

        for (int i = 0; i < 132; i++) {
            ASSERT_EQ(sk_1.raw[i], sk_2.raw[i]);
        }
    }

    void get_test_tx_hash(uint8_t msg_hash[32], uint8_t test_idx) {
        std::vector<uint8_t> msg;

        switch (test_idx) {
            case 0:
                msg = std::vector<uint8_t>(
                        {
                                0x00, 0x01,
                                // Source Address
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                // Fee
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                // Dst Address 0
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                // Amount 0
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        });
                ASSERT_EQ(96, msg.size());
                __sha256(msg_hash, msg.data(), (uint16_t) msg.size());
                break;
            case 1:
                msg = std::vector<uint8_t>(
                        {
                                0x00, 0x02,
                                // Source Address
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                // Fee
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                // Dst Address 0
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                                // Amount 0
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                // Dst Address 1
                                0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                                0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                                0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                                0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                                0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
                                // Amount 1
                                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        });
                ASSERT_EQ(143, msg.size());
                break;
            case 2:
                msg = std::vector<uint8_t>(
                        {
                                0x03, 0x01,
                                // Source Address
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                                // Fee
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        });
                for (uint8_t i = 0; i < 80; i++) {
                    msg.push_back(i);
                }
                ASSERT_EQ(129, msg.size());
                break;
            default:
                ASSERT_TRUE(0);
        }

        get_qrltx_hash((qrltx_t*) msg.data(), msg_hash);
    }

    TEST(XMSS, digest_idx) {
        const std::vector<uint8_t> sk_seed(SZ_SKSEED);      // This should be coming from the SDK

        const uint8_t index = 0;

        uint8_t msg_hash[32];
        get_test_tx_hash(msg_hash, 0);

        std::cout << std::endl;

        xmss_gen_keys(&N_DATA.sk, sk_seed.data());
        for (uint16_t idx = 0; idx < XMSS_NUM_NODES; idx++) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];
            xmss_gen_keys_2_get_nodes(wots_buffer, N_DATA.xmss_nodes + idx * WOTS_N, &N_DATA.sk, idx);
        }

        dump_hex("LEDGER SKSEED     :", N_DATA.sk.seed, 32);

        xmss_digest_t msg_digest;
        xmss_digest(&msg_digest, msg_hash, &N_DATA.sk, index);

        dump_hex("LEDGER DIGEST HASH:", msg_digest.hash, 32);
        dump_hex("LEDGER RANDOMNESS :", msg_digest.randomness, 32);
        dump_hex("LEDGER DIGEST RAW :", msg_digest.raw, 64);
    }

    void generic_test_sign(uint8_t test_idx, uint8_t xmss_idx) {
        const std::vector<uint8_t> sk_seed(SZ_SKSEED);      // This should be coming from the SDK

        uint8_t msg_hash[32];
        get_test_tx_hash(msg_hash, test_idx);

        const uint8_t index = xmss_idx;

        std::cout << std::endl;

        xmss_gen_keys(&N_DATA.sk, sk_seed.data());
        for (uint16_t idx = 0; idx < XMSS_NUM_NODES; idx++) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];
            xmss_gen_keys_2_get_nodes(wots_buffer, N_DATA.xmss_nodes + idx * WOTS_N, &N_DATA.sk, idx);
        }

        xmss_signature_t sig_ledger;
        xmss_sign(&sig_ledger, msg_hash, &N_DATA.sk, N_DATA.xmss_nodes, index);

        dump_hex("LEDGER:", sig_ledger.randomness, 32);
        dump_hex("LEDGER:", sig_ledger.wots_sig, WOTS_SIGSIZE);
        dump_hex("LEDGER:", sig_ledger.auth_path, XMSS_AUTHPATHSIZE);

        std::cout << std::endl;

        xmss_signature_t sig_qrllib;

        xmss_params params{};
        xmss_set_params(&params, XMSS_N, XMSS_H, XMSS_W, XMSS_K);

        nvset(&N_DATA.sk.index, NtoHL(index));

        xmss_Signmsg(eHashFunction::SHA2_256,
                     &params,
                     N_DATA.sk.raw,
                     sig_qrllib.raw,
                     (uint8_t *) msg_hash, 32);

        dump_hex("QRLLIB:", sig_qrllib.randomness, 32);
        dump_hex("QRLLIB:", sig_qrllib.wots_sig, WOTS_SIGSIZE);
        dump_hex("QRLLIB:", sig_qrllib.auth_path, XMSS_AUTHPATHSIZE);

        ASSERT_EQ(sig_ledger.index, sig_qrllib.index);

        for (int i = 0; i < 32; i++) {
            ASSERT_EQ(sig_ledger.randomness[i], sig_qrllib.randomness[i]);
        }
        for (int i = 0; i < WOTS_SIGSIZE; i++) {
            ASSERT_EQ(sig_ledger.wots_sig[i], sig_qrllib.wots_sig[i]);
        }
        for (int i = 0; i < XMSS_AUTHPATHSIZE; i++) {
            ASSERT_EQ(sig_ledger.auth_path[i], sig_qrllib.auth_path[i]);
        }
    }

    TEST(XMSS, sign_idx_tc0_idx0) {
        generic_test_sign(0, 0);
    }

    TEST(XMSS, sign_idx_tc1_idx5) {
        generic_test_sign(1, 5);
    }

    TEST(XMSS, sign_idx_tc2_idx10) {
        generic_test_sign(2, 10);
    }

    TEST(XMSS, sign_idx_xmssfast) {
        const std::vector<uint8_t> sk_seed(SZ_SKSEED);      // This should be coming from the SDK

        const std::vector<uint8_t> msg(32);
        get_test_tx_hash((uint8_t *) msg.data(), 0);

        const uint8_t index = 5;

        std::cout << std::endl;

        xmss_gen_keys(&N_DATA.sk, sk_seed.data());
        for (uint16_t idx = 0; idx < XMSS_NUM_NODES; idx++) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];
            xmss_gen_keys_2_get_nodes(wots_buffer, N_DATA.xmss_nodes + idx * WOTS_N, &N_DATA.sk, idx);
        }

        xmss_signature_t sig_ledger;
        xmss_sign(&sig_ledger, msg.data(), &N_DATA.sk, N_DATA.xmss_nodes, index);

        dump_hex("LEDGER:", sig_ledger.randomness, 32);
        dump_hex("LEDGER:", sig_ledger.wots_sig, WOTS_SIGSIZE);
        dump_hex("LEDGER:", sig_ledger.auth_path, XMSS_AUTHPATHSIZE);

        std::cout << std::endl;

        XmssFast _xmssFast(sk_seed, 8, eHashFunction::SHA2_256);
        _xmssFast.setIndex(index);

        auto sig_qrllib = _xmssFast.sign(msg);

        dump_hex("QRLLIB:", _xmssFast.getAddress().data(), _xmssFast.getAddress().size());

        std::cout << sig_qrllib.size() << std::endl;
        dump_hex("QRLLIB:", sig_qrllib.data() + 4, 32);
        dump_hex("QRLLIB:", sig_qrllib.data() + 36, 2144);


//    dump_hex("QRLLIB:", sig_qrllib.randomness, 32);
//    dump_hex("QRLLIB:", sig_qrllib.wots_sig, WOTS_SIGSIZE);
//    dump_hex("QRLLIB:", sig_qrllib.auth_path, XMSS_AUTHPATHSIZE);
//
//    ASSERT_EQ(sig_ledger.index, sig_qrllib.index);
//
//    for (int i = 0; i<32; i++) {
//        ASSERT_EQ(sig_ledger.randomness[i], sig_qrllib.randomness[i]);
//    }
//    for (int i = 0; i<WOTS_SIGSIZE; i++) {
//        ASSERT_EQ(sig_ledger.wots_sig[i], sig_qrllib.wots_sig[i]);
//    }
//    for (int i = 0; i<XMSS_AUTHPATHSIZE; i++) {
//        ASSERT_EQ(sig_ledger.auth_path[i], sig_qrllib.auth_path[i]);
//    }
    }

    TEST(XMSS, sign_incremental_idx) {
        const std::vector<uint8_t> sk_seed(SZ_SKSEED);      // This should be coming from the SDK

        const std::vector<uint8_t> msg(32);
        get_test_tx_hash((uint8_t *) msg.data(), 0);

        const uint8_t index = 5;

        std::cout << std::endl;

        xmss_gen_keys(&N_DATA.sk, sk_seed.data());
        for (uint16_t idx = 0; idx < XMSS_NUM_NODES; idx++) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];
            xmss_gen_keys_2_get_nodes(wots_buffer, N_DATA.xmss_nodes + idx * WOTS_N, &N_DATA.sk, idx);
        }

        xmss_signature_t sig_ledger;
        xmss_sign(&sig_ledger, msg.data(), &N_DATA.sk, N_DATA.xmss_nodes, index);

        dump_hex("LEDGER:", sig_ledger.randomness, 32);
        dump_hex("LEDGER:", sig_ledger.wots_sig, WOTS_SIGSIZE);
        dump_hex("LEDGER:", sig_ledger.auth_path, XMSS_AUTHPATHSIZE);

        std::cout << std::endl;
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        xmss_signature_t sig_incremental;
        uint8_t *p = sig_incremental.raw;
        xmss_sig_ctx_t ctx;

        xmss_sign_incremental_init(&ctx, msg.data(), &N_DATA.sk, N_DATA.xmss_nodes, index);

        EXPECT_FALSE(xmss_sign_incremental(&ctx, p, &N_DATA.sk, index));
        EXPECT_EQ(ctx.sig_chunk_idx, 1);
        EXPECT_EQ(ctx.written, 164);
        p += ctx.written;

        for (int i = 1; i < 10; i++) {
            EXPECT_FALSE(xmss_sign_incremental(&ctx, p, &N_DATA.sk, index));
            EXPECT_EQ(ctx.sig_chunk_idx, i + 1);
            EXPECT_EQ(ctx.written, 224);
            p += ctx.written;
        }

        EXPECT_TRUE(xmss_sign_incremental_last(&ctx, p, &N_DATA.sk, index));
        EXPECT_EQ(ctx.sig_chunk_idx, 11);
        EXPECT_EQ(ctx.written, 224);
        p += ctx.written;

        dump_hex("QRLLIB:", sig_incremental.randomness, 32);
        dump_hex("QRLLIB:", sig_incremental.wots_sig, WOTS_SIGSIZE);
        dump_hex("QRLLIB:", sig_incremental.auth_path, XMSS_AUTHPATHSIZE);

        ASSERT_EQ(sig_ledger.index, sig_incremental.index);

        for (int i = 0; i < 32; i++) {
            ASSERT_EQ(sig_ledger.randomness[i], sig_incremental.randomness[i]);
        }
        for (int i = 0; i < WOTS_SIGSIZE; i++) {
            ASSERT_EQ(sig_ledger.wots_sig[i], sig_incremental.wots_sig[i]);
        }
        for (int i = 0; i < XMSS_AUTHPATHSIZE; i++) {
            ASSERT_EQ(sig_ledger.auth_path[i], sig_incremental.auth_path[i]);
        }
    }

}
