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
cmake_minimum_required(VERSION 2.8)
project(ledger-qrl)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required

add_subdirectory(cmake/gtest)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src
)

###############

file(GLOB_RECURSE TESTS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        )

file(GLOB_RECURSE APP_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.cpp
        )

file(GLOB_RECURSE LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/*.cpp
        )

file(GLOB_RECURSE QRLLIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/*.cpp
        )

###############
set(BUILD_TESTS OFF CACHE BOOL "Enables tests")
add_subdirectory(deps/qrllib)

include(CTest)
enable_testing()

add_executable(ledger_qrl_tests
        ${TESTS_SRC}
        ${LIB_SRC}
        )

target_include_directories( ledger_qrl_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/qrllib/src/qrl
        )

target_link_libraries(ledger_qrl_tests
        gtest_main
        qrllib)

add_test(ledger_qrl_tests ${PROJECT_BINARY_DIR}/tests)

###############

add_executable(main_example
        ${LIB_SRC}
        ${APP_SRC}
        )

target_link_libraries(main_example)
