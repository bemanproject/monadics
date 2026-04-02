// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/transform.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("not-supported") {}

TEMPLATE_TEST_CASE_SIG("value-is-always-lvalue-ref",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           int*&,
                           [](int&&) { return 0; },
                           false
                       ),
                       (int*&&, [](int&&) { return 0; }, false)) {
    STATIC_REQUIRE(transformable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
