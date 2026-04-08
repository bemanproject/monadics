// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lib_a.hpp"

#include <beman/monadics/monadics.hpp>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

namespace integration {

std::optional<int> process_with_headers(std::optional<int> input) {
    namespace bms = beman::monadics;
    return input | bms::and_then([](int v) { return std::optional{v * 2}; });
}

} // namespace integration
