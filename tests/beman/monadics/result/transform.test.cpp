// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

struct SecondError {
    [[nodiscard]] friend constexpr bool operator==(SecondError, SecondError) = default;
};

template<typename T, typename E>
struct RebindChangesError {
    bool ok{true};
    T v{};
    E e{};
};

template<typename T, typename E>
struct beman::monadics::box_traits<RebindChangesError<T, E>> {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = RebindChangesError<U, SecondError>;

    template<typename F>
    using rebind_error = RebindChangesError<T, F>;

    [[nodiscard]] static constexpr bool has_value(const RebindChangesError<T, E>& b) noexcept { return b.ok; }

    [[nodiscard]] static constexpr decltype(auto) value(auto&& b) { return std::forward<decltype(b)>(b).v; }

    [[nodiscard]] static constexpr decltype(auto) error(auto&& b) { return std::forward<decltype(b)>(b).e; }

    [[nodiscard]] static constexpr RebindChangesError<T, E> make(T v) { return {true, v, E{}}; }

    [[nodiscard]] static constexpr RebindChangesError<T, E> make_error(E e) { return {false, T{}, e}; }
};

namespace beman::monadics::tests {

TEST_CASE("void-value-return-non-void-value") {
    constexpr auto result = Result<void, int>{} | transform([]() { return 2.0; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<double, int>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == 2.0);
}

TEST_CASE("void-value-return-void-value") {
    constexpr auto result = Result<void, int>{} | transform([]() { return; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, int>>);
    STATIC_REQUIRE(result.has_value() == true);
}

TEST_CASE("value-return-different-value") {
    constexpr auto result = Result<int, double>{10} | transform([](auto) { return std::string_view{"some"}; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<std::string_view, double>>);
    STATIC_REQUIRE(result.has_value());
    STATIC_REQUIRE(result.value() == "some");
}

TEST_CASE("value-return-void-value") {
    constexpr auto result = Result<int, double>{10} | transform([](auto) { return; });

    STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
    STATIC_REQUIRE(result.has_value());
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](int&) { return 0; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](int&&) { return 0; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](int&&) { return 0; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](int&) { return 0; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const int&) { return 0; },
                           true
                       ),
                       (const Result<int, double>&, [](int&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

TEST_CASE("refuses-a-box-whose-rebind-rewrites-the-error-side") {
    using Box = RebindChangesError<int, SecondError>;

    STATIC_REQUIRE(box<Box>);
    STATIC_REQUIRE(transformable<Box, decltype([](int v) { return v * 2; })>);
    STATIC_REQUIRE_FALSE(transformable<RebindChangesError<int, int>, decltype([](int v) { return v * 2; })>);
}

} // namespace beman::monadics::tests
