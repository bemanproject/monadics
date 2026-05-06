// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <beman/monadics/monadics.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("chained-and-then-success") {
    constexpr auto result =
        CURLcode{CURLE_OK} | and_then([] { return CURLcode{CURLE_OK}; }) | and_then([] { return CURLcode{CURLE_OK}; });

    STATIC_REQUIRE(result == CURLE_OK);
}

TEST_CASE("chained-and-then-first-fails") {
    constexpr auto result = CURLcode{CURLE_FAILED_INIT}
                          | and_then([] { return CURLcode{CURLE_OK}; })
                          | and_then([] { return CURLcode{CURLE_OK}; });

    STATIC_REQUIRE(result == CURLE_FAILED_INIT);
}

TEST_CASE("chained-and-then-second-fails") {
    constexpr auto result = CURLcode{CURLE_OK}
                          | and_then([] { return CURLcode{CURLE_NOT_BUILT_IN}; })
                          | and_then([] { return CURLcode{CURLE_OK}; });

    STATIC_REQUIRE(result == CURLE_NOT_BUILT_IN);
}

} // namespace beman::monadics::tests
