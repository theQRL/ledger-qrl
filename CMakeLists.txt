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
project(ledger-qrl VERSION 0.0.0)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required

if (APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
endif ()

find_package(OpenSSL)

add_subdirectory(cmake/gtest)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/src/libxmss
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/src/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src
        ${OPENSSL_INCLUDE_DIR}
)

#add_definitions(-DHASH_DEBUG=1 -DDEBUG_XMSS=1)
#add_definitions(-DDEBUG_XMSS=1)

###############

file(GLOB_RECURSE TESTS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/lib/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/libxmss/*.cpp
        )

file(GLOB_RECURSE LIBXMSS_SRC
        src/ledger/src/libxmss/*.c
        src/ledger/src/lib/*.c
        )

###############
set(BUILD_TESTS OFF CACHE BOOL "Enables tests")
add_subdirectory(deps/qrllib)

add_library(xmsslib STATIC
        ${LIBXMSS_SRC})
target_include_directories(xmsslib PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/ledger-zxlib/include
        )
target_link_libraries(xmsslib
        qrllib
        ${OPENSSL_LIBRARIES})

enable_testing()

add_executable(test_qrl
        ${TESTS_SRC}
        )

target_include_directories(test_qrl PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src/qrl
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/ledger-zxlib/include
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        )

target_link_libraries(test_qrl
        gtest_main
        qrllib
        xmsslib
        ${OPENSSL_LIBRARIES})

add_test(QRL_TESTS test_qrl)

###############################################################
# Force tests to depend from app compiling
###############################################################
add_custom_target(ledger_app
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/nanocli.sh make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
