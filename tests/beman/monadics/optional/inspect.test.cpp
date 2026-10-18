// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

namespace {

using O = std::optional<int>;

inline constexpr auto by_const_ref = [](const int&) {
};
inline constexpr auto by_value = [](int) {
};
inline constexpr auto by_ref = [](int&) {
};
inline constexpr auto by_rvalue = [](int&&) {
};
inline constexpr auto nullary = [] {
};

} // namespace

TEST_CASE("runs-on-the-value-and-hands-the-box-back") {
    int seen{};
    O box{10};

    const auto same = box | inspect([&seen](int v) { seen = v; });

    REQUIRE(seen == 10);
    REQUIRE(same == box);
}

TEST_CASE("skipped-when-empty") {
    int seen{};

    const auto same = O{} | inspect([&seen](int v) { seen = v; });

    REQUIRE(seen == 0);
    REQUIRE(same == O{});
}

TEST_CASE("survives-an-rvalue-box") {
    int seen{};

    const auto moved = O{20} | inspect([&seen](int v) { seen = v; });

    REQUIRE(seen == 20);
    REQUIRE(moved == O{20});
}

TEST_CASE("chains") {
    int seen{};

    const auto result = O{5} | inspect([&seen](int v) { seen = v; }) | transform([](int v) { return v + 1; });

    REQUIRE(seen == 5);
    REQUIRE(result == O{6});
}

TEMPLATE_TEST_CASE_SIG("observes-without-consuming",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (O&, by_const_ref, true),
                       (O&, by_value, true),
                       (O&, by_ref, false),
                       (O&&, by_rvalue, false)) {
    STATIC_REQUIRE(inspectable<Box, decltype(Fn)> == Expected);
}

TEST_CASE("a-handler-is-never-probed-at-an-arity-it-refuses") {
    STATIC_REQUIRE(inspectable<O&, decltype(by_value)>);
    STATIC_REQUIRE(inspect_errorable<O&, decltype(nullary)>);
}

TEST_CASE("inspect_error-takes-nothing-when-nothing-is-stored") {
    int fired{};

    const auto same = O{} | inspect_error([&fired] { fired = 1; });

    REQUIRE(fired == 1);
    REQUIRE(same == O{});
    STATIC_REQUIRE(inspect_errorable<O&, decltype(nullary)>);
}

TEST_CASE("inspect_error-skipped-when-engaged") {
    int fired{};

    const auto same = O{10} | inspect_error([&fired] { fired = 1; });

    REQUIRE(fired == 0);
    REQUIRE(same == O{10});
}

} // namespace beman::monadics::tests
