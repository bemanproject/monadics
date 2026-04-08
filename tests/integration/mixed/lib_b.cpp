// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// import must appear before any declarations in a non-module translation unit.
#include "lib_b.hpp"

import beman.monadics;

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

namespace integration {

std::optional<int> process_with_modules(std::optional<int> input) {
    namespace bms = beman::monadics;
    return input | bms::transform([](int v) { return v + 10; });
}

} // namespace integration
