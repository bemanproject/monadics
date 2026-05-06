// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <beman/monadics/detail/and_then.hpp>

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    constexpr auto result = CURLcode{CURLE_OK} | and_then([] { return CURLE_OK; });

    STATIC_REQUIRE(result == CURLE_OK);
}

TEST_CASE("with-error") {
    constexpr auto result = CURLcode{CURLE_COULDNT_RESOLVE_PROXY} | and_then([] { return CURLE_OK; });

    STATIC_REQUIRE(result == CURLE_COULDNT_RESOLVE_PROXY);
}

} // namespace beman::monadics::tests
