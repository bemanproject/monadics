// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

template<typename T, typename E>
struct RebindErrorForcesValue {
    bool ok{true};
    T v{};
    E e{};
};

template<typename T, typename E>
struct beman::monadics::box_traits<RebindErrorForcesValue<T, E>> {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = RebindErrorForcesValue<U, E>;

    template<typename F>
    using rebind_error = RebindErrorForcesValue<int, F>;

    [[nodiscard]] static constexpr bool has_value(const RebindErrorForcesValue<T, E>& b) noexcept { return b.ok; }

    [[nodiscard]] static constexpr decltype(auto) value(auto&& b) { return std::forward<decltype(b)>(b).v; }

    [[nodiscard]] static constexpr decltype(auto) error(auto&& b) { return std::forward<decltype(b)>(b).e; }

    [[nodiscard]] static constexpr RebindErrorForcesValue<T, E> make(T v) { return {true, v, E{}}; }

    [[nodiscard]] static constexpr RebindErrorForcesValue<T, E> make_error(E e) { return {false, T{}, e}; }
};

namespace beman::monadics::tests {

TEST_CASE("void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = Result<void, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = Result<void, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<void, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEST_CASE("non-void-value") {
    constexpr auto fn = [](int v) {
        return v * 2.0;
    };

    SECTION("with-value") {
        constexpr auto result = Result<char, int>{} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<char, double>>);
        STATIC_REQUIRE(result.has_value());
    }

    SECTION("without-value") {
        constexpr auto result = Result<char, int>{10} | transform_error(fn);

        STATIC_REQUIRE(std::same_as<decltype(result), const Result<char, double>>);
        STATIC_REQUIRE(result.has_value() == false);
        STATIC_REQUIRE(result.error() == 20.0);
    }
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           Result<int, double>&,
                           [](double&) { return 0.0; },
                           true
                       ),
                       (
                           Result<int, double>&,
                           [](double&&) { return 0.0; },
                           false
                       ),
                       (
                           Result<int, double>&&,
                           [](double&&) { return 0.0; },
                           true
                       ),
                       (
                           Result<int, double>&&,
                           [](double&) { return 0.0; },
                           false
                       ),
                       (
                           const Result<int, double>&,
                           [](const double&) { return 0.0; },
                           true
                       ),
                       (const Result<int, double>&, [](double&) { return 0.0; }, false)) {
    STATIC_REQUIRE(transform_errorable<Box, decltype(Fn)> == Expected);
}

TEST_CASE("refuses-a-box-whose-rebind-error-rewrites-the-value-side") {
    using Box = RebindErrorForcesValue<double, int>;

    STATIC_REQUIRE(box<Box>);
    STATIC_REQUIRE(transform_errorable<RebindErrorForcesValue<int, int>, decltype([](int e) { return e * 2.0; })>);
    STATIC_REQUIRE_FALSE(transform_errorable<Box, decltype([](int e) { return e * 2.0; })>);
}

} // namespace beman::monadics::tests
