// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/compose.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

inline constexpr auto twice = [](int v) {
    return v * 2;
};
inline constexpr auto inc = [](int v) {
    return v + 1;
};
inline constexpr auto nine = [] {
    return 9;
};

} // namespace

TEST_CASE("applies-f-then-g") {
    STATIC_REQUIRE(compose(inc, twice)(3) == 7);
    STATIC_REQUIRE(compose(twice, inc)(3) == 8);
}

TEST_CASE("a-void-result-reaches-g-as-no-argument") {
    constexpr auto result = [] {
        int seen = 0;
        auto r = compose(nine, [&](int v) { seen = v; })(4);
        return std::pair{seen, r};
    }();
    STATIC_REQUIRE(result.first == 4);
    STATIC_REQUIRE(result.second == 9);
}

TEST_CASE("callable-more-than-once") {
    constexpr auto h = compose(inc, twice);
    STATIC_REQUIRE(h(1) + h(2) == 8);
}

TEST_CASE("a-nullary-f-composes") {
    STATIC_REQUIRE(compose(twice, nine)() == 18);
}

TEST_CASE("only-a-chain-that-fits-is-invocable") {
    STATIC_REQUIRE(std::invocable<decltype(compose(inc, twice)), int>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(compose(inc, twice))>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(compose(inc, twice)), const char*>);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(compose(nine, twice)), int>);
}

} // namespace beman::monadics::detail::tests
