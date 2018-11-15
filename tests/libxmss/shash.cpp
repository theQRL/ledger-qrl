// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <gmock/gmock.h>
#include <qrl/misc.h>
#include <libxmss/shash.h>

namespace {
TEST(HASHING, prf_check_1) {
    shash_input_t h_in{};
    std::vector<uint8_t> prf_out(32);

    shash96(prf_out.data(), &h_in);

    std::string expected_output{"2ea9ab9198d1638007400cd2c3bef1cc745b864b76011a0e1bc52180ac6452d4"};

    EXPECT_EQ(expected_output, bin2hstr(prf_out));
}

TEST(HASHING, prf_check_2) {
    shash_input_t h_in{};
    h_in.seed_gen.cdr = 1;
    std::vector<uint8_t> prf_out(32);

    shash96(prf_out.data(), &h_in);

    std::string expected_output{"b83349f8a55a7e8c46b8620ab012c6e1cc126a75b063732762e959c4ceab5925"};

    EXPECT_EQ(expected_output, bin2hstr(prf_out));
}

}
