// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

namespace {

using Box = Result<double, int>;

} // namespace

TEST_CASE("runs-on-the-value-and-hands-the-box-back") {
    double seen{};
    const Box box{10.0};

    const auto same = box | inspect([&seen](double v) { seen = v; });

    REQUIRE(seen == 10.0);
    REQUIRE(same == box);
}

TEST_CASE("inspect_error-receives-the-stored-error") {
    int seen{};
    const Box box{7};

    const auto same = box | inspect_error([&seen](int e) { seen = e; });

    REQUIRE(seen == 7);
    REQUIRE(same == box);
}

TEST_CASE("each-is-skipped-on-the-other-case") {
    double seen{};
    int errs{};

    const auto engaged = Box{10.0} | inspect_error([&errs](int e) { errs = e; });
    const auto failed = Box{7} | inspect([&seen](double v) { seen = v; });

    REQUIRE(errs == 0);
    REQUIRE(seen == 0.0);
    REQUIRE(engaged == Box{10.0});
    REQUIRE(failed == Box{7});
}

} // namespace beman::monadics::tests
