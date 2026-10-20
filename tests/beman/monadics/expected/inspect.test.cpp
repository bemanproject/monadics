// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

namespace {

using Box = std::expected<int, int>;

} // namespace

TEST_CASE("runs-on-the-value-and-hands-the-box-back") {
    int seen{};
    const Box box{10};

    const auto same = box | inspect([&seen](int v) { seen = v; });

    REQUIRE(seen == 10);
    REQUIRE((same == box));
}

TEST_CASE("skipped-when-failed") {
    int seen{};
    const Box box{std::unexpect, 7};

    const auto same = box | inspect([&seen](int v) { seen = v; });

    REQUIRE(seen == 0);
    REQUIRE((same == box));
}

TEST_CASE("inspect_error-receives-the-stored-error") {
    int seen{};
    const Box box{std::unexpect, 7};

    const auto same = box | inspect_error([&seen](int e) { seen = e; });

    REQUIRE(seen == 7);
    REQUIRE((same == box));
}

TEST_CASE("inspect_error-skipped-when-engaged") {
    int seen{};

    const auto same = Box{10} | inspect_error([&seen](int e) { seen = e; });

    REQUIRE(seen == 0);
    REQUIRE((same == Box{10}));
}

TEST_CASE("a-nullary-handler-is-rejected-where-an-error-is-stored") {
    STATIC_REQUIRE(inspect_errorable<Box&, decltype([](int) {})>);
    STATIC_REQUIRE_FALSE(inspect_errorable<Box&, decltype([] {})>);
}

TEST_CASE("chains-with-the-other-operations") {
    int seen{};
    int errs{};

    const auto result = Box{std::unexpect, 7}
                      | inspect([&seen](int v) { seen = v; })
                      | inspect_error([&errs](int e) { errs = e; })
                      | or_else([](int e) { return Box{e * 10}; });

    REQUIRE(seen == 0);
    REQUIRE(errs == 7);
    REQUIRE((result == Box{70}));
}

} // namespace beman::monadics::tests
