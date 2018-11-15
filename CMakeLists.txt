#*******************************************************************************
#*   (c) 2018 ZondaX GmbH
#*
#*  Licensed under the Apache License, Version 2.0 (the "License");
#*  you may not use this file except in compliance with the License.
#*  You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#*  Unless required by applicable law or agreed to in writing, software
#*  distributed under the License is distributed on an "AS IS" BASIS,
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*  See the License for the specific language governing permissions and
#*  limitations under the License.
#********************************************************************************
cmake_minimum_required(VERSION 3.0)
project(ledger-qrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

if (APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
endif()

find_package(OpenSSL)

add_subdirectory(cmake/gtest)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/libxmss
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src
        ${OPENSSL_INCLUDE_DIR}
)

#add_definitions(-DHASH_DEBUG=1 -DDEBUG_XMSS=1)
#add_definitions(-DDEBUG_XMSS=1)

###############

file(GLOB_RECURSE TESTS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/libledger/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/libxmss/*.cpp
        )

file(GLOB_RECURSE LIBXMSS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/libxmss/*.c
        )

###############
set(BUILD_TESTS OFF CACHE BOOL "Enables tests")
add_subdirectory(deps/qrllib)

add_library(xmsslib STATIC
        ${LIBXMSS_SRC})
target_include_directories( xmsslib PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/ledger-zxlib/include
        )
target_link_libraries(xmsslib
        qrllib
        ${OPENSSL_LIBRARIES})

enable_testing()

add_executable(ledger_qrl_tests
        ${TESTS_SRC}
        )

target_include_directories( ledger_qrl_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src/qrl
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/ledger-zxlib/include
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )

target_link_libraries(ledger_qrl_tests
        gtest_main
        qrllib
        xmsslib
        ${OPENSSL_LIBRARIES})

add_test(QRL_TESTS ledger_qrl_tests)
