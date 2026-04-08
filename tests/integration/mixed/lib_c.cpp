// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lib_c.hpp"

#include "lib_a.hpp"
#include "lib_b.hpp"

namespace integration {

int combined_result() {
    const auto from_a = process_with_headers(std::optional{5});
    const auto from_b = process_with_modules(std::optional{5});
    return from_a.value_or(0) + from_b.value_or(0);
}

} // namespace integration
