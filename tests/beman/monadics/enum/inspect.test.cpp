// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("a-void-value-channel-takes-a-nullary-handler") {
    int fired{};

    const auto same = CURLcode{CURLE_OK} | inspect([&fired] { fired = 1; });

    REQUIRE(fired == 1);
    REQUIRE(same == CURLE_OK);
    STATIC_REQUIRE(inspectable<CURLcode&, decltype([] {})>);
}

TEST_CASE("skipped-when-failed") {
    int fired{};

    const auto same = CURLcode{CURLE_NOT_BUILT_IN} | inspect([&fired] { fired = 1; });

    REQUIRE(fired == 0);
    REQUIRE(same == CURLE_NOT_BUILT_IN);
}

TEST_CASE("inspect_error-receives-the-code") {
    CURLcode seen{CURLE_OK};

    const auto same = CURLcode{CURLE_NOT_BUILT_IN} | inspect_error([&seen](CURLcode e) { seen = e; });

    REQUIRE(seen == CURLE_NOT_BUILT_IN);
    REQUIRE(same == CURLE_NOT_BUILT_IN);
}

} // namespace beman::monadics::tests
