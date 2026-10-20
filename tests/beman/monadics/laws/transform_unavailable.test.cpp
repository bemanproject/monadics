// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures/raw_ptr.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("raw_ptr") {
    using namespace raw_box;

    STATIC_REQUIRE_FALSE(transformable<Box&, decltype([](int& a) { return a; })>);
    STATIC_REQUIRE_FALSE(transform_errorable<Box&, decltype([](unit) { return unit{}; })>);
}

} // namespace beman::monadics::tests
