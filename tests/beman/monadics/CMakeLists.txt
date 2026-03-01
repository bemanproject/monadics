# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(Catch2 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

set(test_global_target "beman.monadics.tests")
add_custom_target(${test_global_target})

add_subdirectory(detail)

set(tests
    optional
    raw_ptr
    shared_ptr
    enum
    expected
)

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
            PRIVATE beman::monadics Catch2::Catch2WithMain
        )
        target_include_directories(
            ${test_target}
            PRIVATE ${CMAKE_CURRENT_LIST_DIR}/${test_name}
        )
        # target_compile_options(
        # ${test_target}
        # PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=10>"
        # )

        catch_discover_tests(${test_target}
          TEST_PREFIX "${test_target}."
        )
    endforeach()
endforeach()
