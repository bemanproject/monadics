# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(examples optional)

foreach(example ${examples})
    add_executable(beman.monadics.examples.${example})
    target_sources(beman.monadics.examples.${example} PRIVATE ${example}.cpp)
    target_link_libraries(
        beman.monadics.examples.${example}
        PRIVATE beman::monadics
    )
endforeach()
