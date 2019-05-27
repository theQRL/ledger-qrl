// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <gmock/gmock.h>

#include <xmss-alt/wots.h>
#include <xmss-alt/wots_internal.h>
#include <xmss-alt/hash_address.h>

#include <wotsp.h>

namespace {
    TEST(WOTSP, expand_seed_zeros) {
        std::vector<uint8_t> seeds_ledger(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> seeds_qrllib(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> sk(WOTS_N);

        wotsp_expand_seed(seeds_ledger.data(), sk.data());

        expand_seed(eHashFunction::SHA2_256, seeds_qrllib.data(), sk.data(), WOTS_N, WOTS_LEN);

        EXPECT_THAT(seeds_qrllib, ::testing::Eq(seeds_ledger));
    }

    TEST(WOTSP, expand_seed2) {
        std::vector<uint8_t> seeds_ledger(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> seeds_qrllib(WOTS_N * WOTS_LEN);

        std::vector<uint8_t> sk{
            0xd6, 0xa6, 0xff, 0x50, 0x79, 0x94, 0x92, 0x1c,
            0x8f, 0x74, 0x24, 0x0a, 0xcd, 0x3d, 0xc8, 0xa7,
            0x32, 0x99, 0x84, 0xe9, 0x2b, 0xed, 0xf1, 0x77,
            0x98, 0xd6, 0xa6, 0xff, 0x50, 0x79, 0x94, 0x92
        };

        wotsp_expand_seed(seeds_ledger.data(), sk.data());

        expand_seed(eHashFunction::SHA2_256, seeds_qrllib.data(), sk.data(), WOTS_N, WOTS_LEN);

        EXPECT_THAT(seeds_qrllib, ::testing::Eq(seeds_ledger));
    }

    TEST(WOTSP, gen_chain) {
        std::vector<uint8_t> chain_ledger(WOTS_N);
        std::vector<uint8_t> chain_qrllib(WOTS_N);
        std::vector<uint8_t> pub_seed(WOTS_N);

        std::cout << std::endl;

        shash_input_t prf_data{};
        PRF_init(&prf_data, SHASH_TYPE_PRF);
        memcpy(prf_data.key, pub_seed.data(), WOTS_N);
        prf_data.adrs.otshash.OTS = 0;

        wotsp_gen_chain(chain_ledger.data(), &prf_data, 0, WOTS_W - 1);

        std::cout << std::endl;

        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};
        gen_chain(eHashFunction::SHA2_256,
                  chain_qrllib.data(),
                  chain_qrllib.data(),
                  0,
                  WOTS_W - 1,
                  &params,
                  pub_seed.data(),
                  ots_addr);

        EXPECT_THAT(chain_qrllib, ::testing::Eq(chain_ledger));
    }

    TEST(WOTSP, pkgen_sha2) {
        std::vector<uint8_t> pk_ledger(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> pk_qrllib(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> sk(WOTS_N);
        std::vector<uint8_t> pub_seed(WOTS_N);

        std::cout << std::endl;

        wotsp_gen_pk(pk_ledger.data(), sk.data(), pub_seed.data(), 0);

        for (int i = 0; i < WOTS_LEN; i++) {
            dump_hex("", pk_ledger.data() + (32 * i), 32);
        }
        std::cout << std::endl;

        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};

        wots_pkgen(eHashFunction::SHA2_256,
                   pk_qrllib.data(),
                   sk.data(),
                   &params,
                   pub_seed.data(),
                   ots_addr);

        EXPECT_THAT(pk_qrllib, ::testing::Eq(pk_ledger));
    }

    TEST(WOTSP, sign_verify) {
        std::vector<uint8_t> pk(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> sk(WOTS_N);
        std::vector<uint8_t> msg(WOTS_N);

        std::vector<uint8_t> sig(WOTS_N * WOTS_LEN);
        std::vector<uint8_t> pub_seed(WOTS_N);

        std::cout << std::endl;

        wotsp_gen_pk(pk.data(), sk.data(), pub_seed.data(), 0);
        wotsp_sign(sig.data(), msg.data(), pub_seed.data(), sk.data(), 0);

        dump_hex("SIG00: ", sig.data(), 32);
        dump_hex("SIG66: ", sig.data() + (32 * 66), 32);

        /////////////////////
        std::vector<uint8_t> pk_ver(WOTS_N * WOTS_LEN);
        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};

        wots_pkFromSig(eHashFunction::SHA2_256,
                       pk_ver.data(),
                       sig.data(),
                       msg.data(),
                       &params,
                       pub_seed.data(),
                       ots_addr);

        EXPECT_THAT(pk, ::testing::Eq(pk_ver));
    }

    TEST(WOTSP, sign_match) {
        std::vector<uint8_t> pk(WOTS_SIGSIZE);
        std::vector<uint8_t> sk(WOTS_N);
        std::vector<uint8_t> msg(WOTS_N);

        std::vector<uint8_t> sig_ledger(WOTS_SIGSIZE);
        std::vector<uint8_t> sig_qrllib(WOTS_SIGSIZE);
        std::vector<uint8_t> pub_seed(WOTS_N);

        uint16_t index = 5;

        pub_seed[1] = 1;

        wotsp_gen_pk(pk.data(), sk.data(), pub_seed.data(), index);
        wotsp_sign(sig_ledger.data(), msg.data(), pub_seed.data(), sk.data(), index);

        /////////////////////
        wots_params params{};
        wots_set_params(&params, WOTS_N, WOTS_W);
        uint32_t ots_addr[8]{};
        setOTSADRS(ots_addr, index);

        wots_sign(eHashFunction::SHA2_256,
                  sig_qrllib.data(),
                  msg.data(),
                  sk.data(),
                  &params,
                  pub_seed.data(),
                  ots_addr
        );

        EXPECT_THAT(sig_ledger, ::testing::Eq(sig_qrllib));
    }
}
