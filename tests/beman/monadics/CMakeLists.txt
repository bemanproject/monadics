# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 3 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

add_library(beman.monadics.tests.helpers INTERFACE)
target_include_directories(
    beman.monadics.tests.helpers
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include
)
target_sources(
    beman.monadics.tests.helpers
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
            FILES
                include/helpers/keeps_return_value_category.hpp
                include/helpers/move_only.hpp
                include/helpers/move_tracker.hpp
)

set(test_global_target "beman.monadics.tests")
add_custom_target(${test_global_target})

add_subdirectory(detail)

set(tests
    optional
    raw_ptr
    shared_ptr
    enum
    result
)

if(BEMAN_MONADICS_HAVE_STD_EXPECTED)
    list(APPEND tests expected)
endif()

foreach(test_dir ${tests})
    set(test_set
        and_then
        or_else
        trait
        transform
        transform_error
        compose
    )

    set(test_dir_target "${test_global_target}.${test_dir}")
    add_custom_target(${test_dir_target})
    add_dependencies(${test_global_target} ${test_dir_target})

    foreach(test_name ${test_set})
        set(test_target "${test_dir_target}.${test_name}")
        add_executable(${test_target})
        add_dependencies(${test_dir_target} ${test_target})

        target_sources(
            ${test_target}
            PRIVATE "${test_dir}/${test_name}.test.cpp"
        )
        target_link_libraries(
            ${test_target}
            PRIVATE
                beman::monadics
                Catch2::Catch2WithMain
                beman.monadics.tests.helpers
        )
        target_include_directories(
            ${test_target}
            PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${test_name}
        )

        catch_discover_tests(${test_target}
          TEST_PREFIX "${test_target}."
        )
    endforeach()
endforeach()
