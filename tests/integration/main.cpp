// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <optional>

#include <beman/monadics/monadics.hpp>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

int main() {
    namespace bms = beman::monadics;

    const auto result = std::optional{42}
                      | bms::and_then([](int v) { return std::optional{v * 2}; })
                      | bms::transform([](int v) { return v + 1; });

    return result.has_value() ? 0 : 1;
}
