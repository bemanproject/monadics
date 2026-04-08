# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30)

project(beman.monadics.integration.find_package CXX)

option(BEMAN_USE_MODULES "" OFF)

set(BEMAN_INTEGRATION_CONSUMER_TARGET
    "beman::monadics"
    CACHE STRING
    "Which beman::monadics_* target the integration executable links against"
)
option(BEMAN_INTEGRATION_USE_IMPORT_SOURCE "" OFF)

find_package(beman.monadics CONFIG REQUIRED)

get_target_property(
    beman_monadics_verify_headers
    beman::monadics_headers
    VERIFY_INTERFACE_HEADER_SETS
)
if(beman_monadics_verify_headers)
    message(
        FATAL_ERROR
        "beman.monadics must not set VERIFY_INTERFACE_HEADER_SETS=ON for consumers."
    )
endif()

get_target_property(
    beman_monadics_interface_features
    beman::monadics_headers
    INTERFACE_COMPILE_FEATURES
)
if(NOT "cxx_std_20" IN_LIST beman_monadics_interface_features)
    message(
        FATAL_ERROR
        "beman.monadics must advertise cxx_std_20 via INTERFACE_COMPILE_FEATURES"
    )
endif()

if(BEMAN_USE_MODULES)
    get_target_property(defs beman::monadics INTERFACE_COMPILE_DEFINITIONS)
    if(NOT "BEMAN_USE_MODULES" IN_LIST defs)
        message(
            FATAL_ERROR
            "beman::monadics must propagate BEMAN_USE_MODULES when built with module support"
        )
    endif()
endif()

if(BEMAN_INTEGRATION_USE_IMPORT_SOURCE)
    set(_integration_source "${CMAKE_CURRENT_SOURCE_DIR}/../main_import.cpp")
else()
    set(_integration_source "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp")
endif()

add_executable(beman.monadics.integration.find_package)
target_sources(
    beman.monadics.integration.find_package
    PRIVATE "${_integration_source}"
)
target_link_libraries(
    beman.monadics.integration.find_package
    PRIVATE ${BEMAN_INTEGRATION_CONSUMER_TARGET}
)
set_target_properties(
    beman.monadics.integration.find_package
    PROPERTIES CXX_SCAN_FOR_MODULES ${BEMAN_USE_MODULES}
)
