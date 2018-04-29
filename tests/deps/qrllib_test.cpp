/*******************************************************************************
*   (c) 2018 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
#include <hashing.h>
#include <misc.h>
//#include <xmss/wots_params.h>
//#include <xmss/hash_local.h>
#include "gtest/gtest.h"

namespace {
    TEST(QRLLIB, sha2_256) {
        const std::string input = "This is a test X";

        auto input_bin = str2bin(input);
        auto output_hashed = sha2_256(input_bin);

        EXPECT_EQ(input_bin.size(), 16);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "54686973206973206120746573742058");
        EXPECT_EQ(bin2hstr(output_hashed), "a11609b2cc5f26619fcc865473246c9ac59861383a3c4edd2433230258afa03b");
    }

    TEST(QRLLIB, sha2_256_1) {
        const std::string input = "This is a test X";

        auto input_bin = str2bin(input);
        auto output_hashed = sha2_256_n(input_bin, 1);

        EXPECT_EQ(input_bin.size(), 16);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "54686973206973206120746573742058");
        EXPECT_EQ(bin2hstr(output_hashed), "a11609b2cc5f26619fcc865473246c9ac59861383a3c4edd2433230258afa03b");
    }

    TEST(QRLLIB, sha2_256_96) {
        auto input_bin = std::vector<uint8_t>(96);
        auto output_hashed = sha2_256(input_bin);

        EXPECT_EQ(input_bin.size(), 96);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                                       "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                                       "0000000000000000000000000000000000000000");
        EXPECT_EQ(bin2hstr(output_hashed), "2ea9ab9198d1638007400cd2c3bef1cc745b864b76011a0e1bc52180ac6452d4");
    }

    TEST(QRLLIB, sha2_256_n) {
        const std::string input = "This is a test X";

        auto input_bin = str2bin(input);
        auto output_hashed = sha2_256_n(input_bin, 16);

        EXPECT_EQ(input_bin.size(), 16);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "54686973206973206120746573742058");
        EXPECT_EQ(bin2hstr(output_hashed), "3be2d7e048d22de2c117465e5b4b819e764352680027c9790a53a7326d62a0fe");
    }

    TEST(QRLLIB, shake128) {
        const std::string input = "This is a test X";
        const size_t hash_size = 32;

        auto input_bin = str2bin(input);
        auto output_hashed = shake128(hash_size, input_bin);

        EXPECT_EQ(input_bin.size(), 16);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "54686973206973206120746573742058");
        EXPECT_EQ(bin2hstr(output_hashed), "02c7654fd239753b787067b1b75523d9bd2c39daa384e4b0d4f91eb78d2a5492");
    }

    TEST(QRLLIB, shake256) {
        const std::string input = "This is a test X";
        const size_t hash_size = 32;

        auto input_bin = str2bin(input);
        auto output_hashed = shake256(hash_size, input_bin);

        EXPECT_EQ(input_bin.size(), 16);
        EXPECT_EQ(output_hashed.size(), 32);

        EXPECT_EQ(bin2hstr(input_bin), "54686973206973206120746573742058");
        EXPECT_EQ(bin2hstr(output_hashed), "b3453cb0cbd37d726a842eb750e6091b15a92efd2695e3191a96d8d07413db04");
    }
}
