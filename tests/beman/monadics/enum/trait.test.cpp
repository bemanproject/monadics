// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("box-trait-for") {
    using Traits = get_box_traits<CURLcode>;
    STATIC_REQUIRE(std::same_as<Traits::value_type, void>);
    STATIC_REQUIRE(std::same_as<Traits::error_type, CURLcode>);
    STATIC_REQUIRE(std::same_as<Traits::rebind<int>, CURLcode>);
    STATIC_REQUIRE(std::same_as<Traits::rebind_error<int>, CURLcode>);
    STATIC_REQUIRE(std::same_as<decltype(Traits::value(CURLcode{})), void>);
    STATIC_REQUIRE(Traits::has_value(CURLcode{CURLE_OK}));
    STATIC_REQUIRE(Traits::has_value(CURLcode{CURLE_NOT_BUILT_IN}) == false);
    STATIC_REQUIRE(Traits::error(CURLcode{}) == CURLcode{});
    STATIC_REQUIRE(Traits::make() == CURLcode{});
    STATIC_REQUIRE(Traits::make_error(CURLE_NOT_BUILT_IN) == CURLE_NOT_BUILT_IN);
}

TEST_CASE("has-error-channel") {
    STATIC_REQUIRE(has_error_channel<CURLcode>);
}

} // namespace beman::monadics::tests
