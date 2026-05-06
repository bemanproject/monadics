// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <beman/monadics/detail/transform.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = CURLcode{CURLE_OK} | transform([] {});

    STATIC_REQUIRE(result == CURLE_OK);
}

TEST_CASE("with-error") {
    constexpr auto result = CURLcode{CURLE_NOT_BUILT_IN} | transform([] {});

    STATIC_REQUIRE(result == CURLE_NOT_BUILT_IN);
}

} // namespace beman::monadics::tests
