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

#include <xmss.h>
#include "nvram.h"
#include <qrl_types.h>

namespace {
    // THESE TESTS GENERATE TEST DATA FOR FAKE EMBEDDED TREES USED IN TEST MODE
    void dump_leaves(std::vector<uint8_t> &sk_seed) {
        xmss_pk_t pk{};
        xmss_sk_t sk{};

        std::cout << std::endl;

        xmss_gen_keys_1_get_seeds(&sk, sk_seed.data());

        uint8_t xmss_nodes[XMSS_NODES_BUFSIZE];
        for (uint16_t idx = 0; idx < XMSS_NUM_NODES; idx++) {
            uint8_t wots_buffer[WOTS_LEN * WOTS_N];

            xmss_gen_keys_2_get_nodes(
                wots_buffer,
                xmss_nodes + idx * WOTS_N,
                &sk,
                idx);
        }

        xmss_gen_keys_3_get_root(xmss_nodes, &sk);

        // Dump leaves
        printf("{\n");
        for (int i = 0; i < 256; i++) {
            printf("{");
            for (int j = 0; j < 32; j++) {
                printf("0x%02X", *(xmss_nodes + 32 * i + j));
                if (j<31) {
                    printf(",");
                }
            }
            if (i<255){
                printf("},\n");
            } else{
                printf("}\n");
            }
        }
        printf("}\n\n\n");
        printf("EXPECTED PK");
        xmss_pk(&pk, &sk);

        dump_hex("PK:", pk.root, 64);
    }

    TEST(XMSS, dump_leaves_zeros) {
        std::vector<uint8_t> sk_seed(48);
        dump_leaves(sk_seed);
    }

    TEST(XMSS, dump_leaves_ones) {
        std::vector<uint8_t> sk_seed(48);
        for (uint8_t i = 0; i < 48u; i++)
            sk_seed[i] = 0xFF;
        dump_leaves(sk_seed);
    }
}
