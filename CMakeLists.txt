# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30...4.3)

project(
    beman.monadics
    DESCRIPTION "Freestanding monadic operations"
    LANGUAGES CXX
    VERSION 0.1.0
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

# for find of beman_install_library and configure_build_telemetry
include(infra/cmake/beman-install-library.cmake)
include(infra/cmake/BuildTelemetryConfig.cmake)

add_library(beman.monadics INTERFACE)
add_library(beman::monadics ALIAS beman.monadics)

target_sources(
    beman.monadics
    PUBLIC FILE_SET HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(
    beman.monadics
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
)

add_subdirectory(include/beman/monadics)

beman_install_library(beman.monadics TARGETS beman.monadics)
configure_build_telemetry()

if(
    (BEMAN_MONADICS_BUILD_TESTS OR BEMAN_MONADICS_BUILD_EXAMPLES)
    AND (CMAKE_CXX_STANDARD GREATER_EQUAL 23)
)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles(
        [[
    #include <expected>

    int main()
    {
      const std::expected<int, int> result;
      return result.value();
    }
    ]]
        BEMAN_MONADICS_HAVE_STD_EXPECTED
    )
endif()

if(BEMAN_MONADICS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/beman/monadics)
endif()

if(BEMAN_MONADICS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
