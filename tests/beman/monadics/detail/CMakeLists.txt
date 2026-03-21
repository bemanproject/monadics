# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(test_dir_target "beman.monadics.tests.detail")
add_custom_target(${test_dir_target})
add_dependencies(${test_global_target} ${test_dir_target})

set(tests
    as_pointer
    decomposable
    get_error_fn
    get_error_type
    get_make_error_fn
    get_make_fn
    get_rebind
    get_rebind_error
    get_value_fn
    get_value_query_fn
    get_value_type
    instance_of
    invoke_with_error
    invoke_with_value
    meta_extract_value_type
    meta_rebind_error
    meta_rebind_value
    rebox_error
    rebox_value
    same_box
    same_unqualified_as
)

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

    catch_discover_tests(${test_target}
      TEST_PREFIX "${test_target}."
    )
endforeach()
