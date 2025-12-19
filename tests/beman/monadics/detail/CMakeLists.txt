# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(test_dir_target "beman.monadics.tests.detail")
add_custom_target(${test_dir_target})
add_dependencies(${test_global_target} ${test_dir_target})

set(tests deduce_value_type)

foreach(test_name ${tests})
    set(test_target "${test_dir_target}.${test_name}")
    add_executable(${test_target})
    add_dependencies(${test_dir_target} ${test_target})

    target_sources(${test_target} PRIVATE "${test_name}.test.cpp")
    target_link_libraries(
        ${test_target}
        PRIVATE beman::monadics Catch2::Catch2WithMain
    )
    target_compile_options(
        ${test_target}
        PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-fconcepts-diagnostics-depth=10>"
    )

    catch_discover_tests(${test_target}
      TEST_PREFIX "${test_target}."
    )
endforeach()
