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
    constexpr auto lifted = [](int a) {
        return Traits::make(a + 1);
    };

    STATIC_REQUIRE((engaged | transform(inc)) == (engaged | and_then(lifted)));
    STATIC_REQUIRE((failed | transform(inc)) == (failed | and_then(lifted)));
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto inc = [](int a) {
        return a + 1;
    };
    constexpr auto lifted = [](int a) {
        return Traits::make(a + 1);
    };

    STATIC_REQUIRE((engaged | transform(inc)) == (engaged | and_then(lifted)));
    STATIC_REQUIRE((failed | transform(inc)) == (failed | and_then(lifted)));
}
#endif

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto inc = [](double a) {
        return a + 1.0;
    };
    constexpr auto lifted = [](double a) {
        return Traits::make(a + 1.0);
    };

    STATIC_REQUIRE((engaged | transform(inc)) == (engaged | and_then(lifted)));
    STATIC_REQUIRE((failed | transform(inc)) == (failed | and_then(lifted)));
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto inc = [](int a) {
        return a + 1;
    };
    const auto lifted = [](int a) {
        return Traits::make(a + 1);
    };

    const Box engaged = make_engaged();
    const Box failed = make_failed();

    REQUIRE(holds_the_same_value(engaged | transform(inc), engaged | and_then(lifted)));
    REQUIRE(holds_the_same_value(failed | transform(inc), failed | and_then(lifted)));
}

} // namespace beman::monadics::tests
