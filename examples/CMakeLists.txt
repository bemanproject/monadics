# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(examples_global_target "beman.monadics.examples")
add_custom_target(${examples_global_target})

set(examples optional)

if(BEMAN_MONADICS_HAVE_STD_EXPECTED)
    list(APPEND examples expected)
endif()

foreach(example ${examples})
    set(example_target "${examples_global_target}.${example}")

    add_executable(${example_target})
    target_sources(${example_target} PRIVATE ${example}.cpp)
    target_link_libraries(${example_target} PRIVATE beman::monadics)
endforeach()
