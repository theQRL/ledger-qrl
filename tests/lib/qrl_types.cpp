// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <gmock/gmock.h>

extern "C"
{
#include "qrl_types.h"
}

namespace {
    TEST(LIB, qrl_types_static_sizes) {
        EXPECT_THAT(sizeof(qrltx_addr_block), ::testing::Eq(47));
        EXPECT_THAT(sizeof(qrltx_slave_block), ::testing::Eq(43));

        EXPECT_THAT(sizeof(qrltx_tx_t), ::testing::Eq(188));
        EXPECT_THAT(sizeof(qrltx_txtoken_t), ::testing::Eq(220));
        EXPECT_THAT(sizeof(qrltx_slave_t), ::testing::Eq(176));

        EXPECT_THAT(sizeof(qrltx_t), ::testing::Eq(222));
    }

    TEST(LIB, qrl_types_invalid) {
        std::vector<uint8_t> data_single_byte{0};
        EXPECT_THAT(get_qrltx_size((qrltx_t *) data_single_byte.data()),
                    ::testing::Eq(-1));
    }

    TEST(LIB, qrl_types_tx) {
        qrltx_t tx;
        tx.type = QRLTX_TX;

        tx.subitem_count = 0;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));

        tx.subitem_count = 1;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(96));

        tx.subitem_count = 2;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(143));

        tx.subitem_count = 3;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(190));

        tx.subitem_count = 4;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));
    }

    TEST(LIB, qrl_types_txtoken) {
        qrltx_t tx;
        tx.type = QRLTX_TXTOKEN;

        tx.subitem_count = 0;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));

        tx.subitem_count = 1;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(128));

        tx.subitem_count = 2;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(175));

        tx.subitem_count = 3;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(222));

        tx.subitem_count = 4;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));
    }

    TEST(LIB, qrl_types_slave) {
        qrltx_t tx;
        tx.type = QRLTX_SLAVE;

        tx.subitem_count = 0;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));

        tx.subitem_count = 1;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(96));

        tx.subitem_count = 2;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(143));

        tx.subitem_count = 3;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(190));

        tx.subitem_count = 4;
        EXPECT_THAT(get_qrltx_size(&tx), ::testing::Eq(-1));
    }

}
