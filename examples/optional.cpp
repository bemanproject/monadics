// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <cstdlib>
#include <optional>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

int main() {
    namespace bms = beman::monadics;

    const auto result = std::optional{10}
                      | bms::and_then([](auto&& v) { return std::optional{std::forward<decltype(v)>(v) * 2.0}; })
                      | bms::transform([](auto&& v) { return static_cast<int>(v); })
                      | bms::and_then([](auto) { return std::optional<std::size_t>{}; })
                      | bms::or_else([]() { return std::optional<std::size_t>{EXIT_SUCCESS}; });
    return result.value_or(EXIT_FAILURE);
}
