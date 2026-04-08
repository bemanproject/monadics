// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <optional>

namespace integration {

// Implemented using beman.monadics via #include.
std::optional<int> process_with_headers(std::optional<int> input);

} // namespace integration
