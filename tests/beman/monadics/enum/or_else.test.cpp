// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/or_else.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           CURLcode&,
                           [](CURLcode&) { return CURLE_OK; },
                           true
                       ),
                       (
                           CURLcode&,
                           [](CURLcode&&) { return CURLE_OK; },
                           false
                       ),
                       (
                           CURLcode&&,
                           [](CURLcode&&) { return CURLE_OK; },
                           true
                       ),
                       (
                           CURLcode&&,
                           [](CURLcode&) { return CURLE_OK; },
                           false
                       ),
                       (
                           const CURLcode&,
                           [](const CURLcode&) { return CURLE_OK; },
                           true
                       ),
                       (const CURLcode&, [](CURLcode&) { return CURLE_OK; }, false)) {
    STATIC_REQUIRE(or_elseable<Box, decltype(Fn)> == Expected);
}

} // namespace beman::monadics::tests
