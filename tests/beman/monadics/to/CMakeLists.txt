# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(test_dir_target "beman.monadics.tests.to")
add_custom_target(${test_dir_target})
add_dependencies(${test_global_target} ${test_dir_target})

set(tests to)

foreach(test_name ${tests})
    set(test_target "${test_dir_target}.${test_name}")
    add_executable(${test_target})
    add_dependencies(${test_dir_target} ${test_target})

    target_sources(${test_target} PRIVATE "${test_name}.test.cpp")
    target_link_libraries(
        ${test_target}
        PRIVATE
            beman::monadics
            beman.monadics.tests.helpers
            Catch2::Catch2WithMain
    )

    catch_discover_tests(${test_target} TEST_PREFIX "${test_target}.")
endforeach()
