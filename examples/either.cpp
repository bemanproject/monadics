// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/monadics.hpp>

#include <cstdio>
#include <cstdlib>
#include <expected>
#include <optional>
#include <string>

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

template<typename T, typename E>
struct beman::monadics::box_traits<std::expected<T, E>> {
    [[nodiscard]] inline static constexpr auto make_error(auto&& e) {
        return std::expected<T, E>{std::unexpected{std::forward<decltype(e)>(e)}};
    }
};

int main() {
    namespace bms = beman::monadics;

    const auto described = bms::either([](int v) { return std::to_string(v); },
                                       [](auto&&) { return std::string{"none"}; },
                                       std::optional<int>{10});

    bms::either([](int v) { std::printf("%d\n", v); }, [] { std::printf("none\n"); }, std::optional<int>{10});

    const auto from_absence =
        bms::either([](int v) { return v; }, [](bms::unit) { return EXIT_FAILURE; }, std::optional<int>{});
    const auto from_error = bms::either([](int v) { return v; },
                                        [](char e) { return static_cast<int>(e); },
                                        std::expected<int, char>{std::unexpected{'e'}});

    const auto widened = bms::either([](int v) -> std::expected<int, char> { return v; },
                                     bms::carry_error,
                                     std::expected<int, char>{10});

    const auto only_value = bms::either([](int v) { return std::optional<int>{v}; }, bms::carry_error, bms::just(10));
    const auto only_error =
        bms::either(bms::carry_value, [](char) { return std::optional<int>{}; }, bms::just_error('e'));

    std::printf("%s %d %d %d %d %d\n",
                described.c_str(),
                from_absence,
                from_error,
                widened.value_or(EXIT_FAILURE),
                only_value.value_or(EXIT_FAILURE),
                only_error.value_or(EXIT_SUCCESS));
    return EXIT_SUCCESS;
}
