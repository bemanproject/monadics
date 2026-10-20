# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(laws
    left_identity
    right_identity
    associativity
    catch_throw
    pass_through
    functor_identity
    functor_composition
    bifunctor_identity
    bifunctor_composition
    transform_agrees_with_and_then
    transform_error_agrees_with_or_else
    transform_unavailable
)

set(laws_target "${test_global_target}.laws")
add_custom_target(${laws_target})
add_dependencies(${test_global_target} ${laws_target})

foreach(law ${laws})
    set(law_target "${laws_target}.${law}")

    add_executable(${law_target})
    add_dependencies(${laws_target} ${law_target})

    target_sources(${law_target} PRIVATE "${law}.test.cpp")
    target_link_libraries(
        ${law_target}
        PRIVATE
            beman::monadics
            Catch2::Catch2WithMain
            beman.monadics.tests.helpers
    )
    target_include_directories(
        ${law_target}
        PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/..
    )

    catch_discover_tests(${law_target} TEST_PREFIX "${law_target}.")
endforeach()
