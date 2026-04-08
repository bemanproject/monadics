# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30)

set(CMAKE_COLOR_DIAGNOSTICS ON)

project(beman.monadics.integration.mixed CXX)

include(FetchContent)

if(NOT DEFINED BEMAN_MONADICS_SOURCE_DIR)
    message(
        FATAL_ERROR
        "BEMAN_MONADICS_SOURCE_DIR must be set to the root of the beman.monadics source tree."
    )
endif()

set(BEMAN_USE_MODULES ON CACHE BOOL "" FORCE)

FetchContent_Declare(beman_monadics SOURCE_DIR "${BEMAN_MONADICS_SOURCE_DIR}")
FetchContent_MakeAvailable(beman_monadics)

add_library(lib_a lib_a.cpp)
target_include_directories(lib_a PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lib_a PUBLIC beman::monadics_headers)

add_library(lib_b lib_b.cpp)
target_include_directories(lib_b PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lib_b PUBLIC beman::monadics_module)
set_target_properties(lib_b PROPERTIES CXX_SCAN_FOR_MODULES ON)

add_library(lib_c lib_c.cpp)
target_include_directories(lib_c PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(lib_c PUBLIC lib_a lib_b)

add_executable(beman.monadics.integration.mixed main.cpp)
target_link_libraries(beman.monadics.integration.mixed PRIVATE lib_c)
