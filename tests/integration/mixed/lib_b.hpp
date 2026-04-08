// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <optional>

namespace integration {

// Implemented using beman.monadics via import.
std::optional<int> process_with_modules(std::optional<int> input);

} // namespace integration
