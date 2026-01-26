# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

set(CMAKE_LINK_DEPENDS_NO_SHARED ${BUILD_SHARED_LIBS})

project(
    beman.monadics
    DESCRIPTION "A Beman Library Monadics"
    LANGUAGES CXX
    VERSION 0.0.1
)

# [CMAKE.SKIP_TESTS]
option(
    BEMAN_MONADICS_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    BEMAN_MONADICS_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

option(
    BEMAN_MONADICS_INSTALL_CONFIG_FILE_PACKAGE
    "Enable creating and installing a CMake config-file package. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

include(infra/cmake/beman-install-library-config.cmake)

add_subdirectory(src/beman/monadics)

beman_install_library(beman.monadics)

if(BEMAN_MONADICS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/beman/monadics)
endif()

if(BEMAN_MONADICS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
