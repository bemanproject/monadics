// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures/enum.hpp"
#include "fixtures/result.hpp"
#if defined(__cpp_lib_expected)
#include "fixtures/expected.hpp"
#endif

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto inc = [](int e) {
        return e + 1;
    };
    constexpr auto lifted = [](int e) {
        return Traits::make_error(e + 1);
    };

    STATIC_REQUIRE((engaged | transform_error(inc)) == (engaged | or_else(lifted)));
    STATIC_REQUIRE((failed | transform_error(inc)) == (failed | or_else(lifted)));
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto to_init = [](Box) {
        return Box{CURLE_FAILED_INIT};
    };
    constexpr auto lifted = [](Box) {
        return Traits::make_error(CURLE_FAILED_INIT);
    };

    STATIC_REQUIRE((engaged | transform_error(to_init)) == (engaged | or_else(lifted)));
    STATIC_REQUIRE((failed | transform_error(to_init)) == (failed | or_else(lifted)));
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto inc = [](int e) {
        return e + 1;
    };
    constexpr auto lifted = [](int e) {
        return Traits::make_error(e + 1);
    };

    STATIC_REQUIRE((engaged | transform_error(inc)) == (engaged | or_else(lifted)));
    STATIC_REQUIRE((failed | transform_error(inc)) == (failed | or_else(lifted)));
}

} // namespace beman::monadics::tests
