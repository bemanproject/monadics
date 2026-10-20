// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures/optional.hpp"
#include "fixtures/result.hpp"
#include "fixtures/shared_ptr.hpp"
#if defined(__cpp_lib_expected)
#include "fixtures/expected.hpp"
#endif

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("optional") {
    using namespace opt_box;

    constexpr auto inc = [](int a) {
        return a + 1;
    };
    constexpr auto dbl = [](int a) {
        return a * 2;
    };
    constexpr auto both = [](int a) {
        return (a + 1) * 2;
    };

    STATIC_REQUIRE((engaged | transform(inc) | transform(dbl)) == (engaged | transform(both)));
    STATIC_REQUIRE((failed | transform(inc) | transform(dbl)) == (failed | transform(both)));
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto inc = [](int a) {
        return a + 1;
    };
    constexpr auto dbl = [](int a) {
        return a * 2;
    };
    constexpr auto both = [](int a) {
        return (a + 1) * 2;
    };

    STATIC_REQUIRE((engaged | transform(inc) | transform(dbl)) == (engaged | transform(both)));
    STATIC_REQUIRE((failed | transform(inc) | transform(dbl)) == (failed | transform(both)));
}
#endif

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto inc = [](double a) {
        return a + 1.0;
    };
    constexpr auto dbl = [](double a) {
        return a * 2.0;
    };
    constexpr auto both = [](double a) {
        return (a + 1.0) * 2.0;
    };

    STATIC_REQUIRE((engaged | transform(inc) | transform(dbl)) == (engaged | transform(both)));
    STATIC_REQUIRE((failed | transform(inc) | transform(dbl)) == (failed | transform(both)));
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto inc = [](int a) {
        return a + 1;
    };
    const auto dbl = [](int a) {
        return a * 2;
    };
    const auto both = [](int a) {
        return (a + 1) * 2;
    };

    const Box engaged = make_engaged();
    const Box failed = make_failed();

    REQUIRE(holds_the_same_value(engaged | transform(inc) | transform(dbl), engaged | transform(both)));
    REQUIRE(holds_the_same_value(failed | transform(inc) | transform(dbl), failed | transform(both)));
}

} // namespace beman::monadics::tests
