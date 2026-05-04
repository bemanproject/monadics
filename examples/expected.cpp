// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <cstdlib>
#include <expected>

template<typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {
    [[nodiscard]] inline static constexpr auto make_error(auto&& e) {
        return std::expected<T, E>{std::unexpected{std::forward<decltype(e)>(e)}};
    }
};

int main() {
    namespace bms = beman::monadics;

    const auto result =
        std::expected<int, int>{10}
        | bms::and_then([](auto&& v) -> std::expected<double, int> { return std::forward<decltype(v)>(v) * 2.0; })
        | bms::transform([](auto&& v) { return static_cast<int>(v); })
        | bms::and_then([](auto) -> std::expected<int, int> { return std::unexpected{5}; })
        | bms::or_else([](auto) -> std::expected<int, char> { return EXIT_SUCCESS; });
    return result.value_or(EXIT_FAILURE);
}
