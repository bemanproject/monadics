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
    constexpr auto dbl = [](int e) {
        return e * 2;
    };
    constexpr auto both = [](int e) {
        return (e + 1) * 2;
    };

    STATIC_REQUIRE((engaged | transform_error(inc) | transform_error(dbl)) == (engaged | transform_error(both)));
    STATIC_REQUIRE((failed | transform_error(inc) | transform_error(dbl)) == (failed | transform_error(both)));
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto to_init = [](Box) {
        return Box{CURLE_FAILED_INIT};
    };
    constexpr auto to_malformat = [](Box) {
        return Box{CURLE_URL_MALFORMAT};
    };
    constexpr auto both = [](Box) {
        return Box{CURLE_URL_MALFORMAT};
    };

    STATIC_REQUIRE((engaged | transform_error(to_init) | transform_error(to_malformat))
                   == (engaged | transform_error(both)));
    STATIC_REQUIRE((failed | transform_error(to_init) | transform_error(to_malformat))
                   == (failed | transform_error(both)));
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto inc = [](int e) {
        return e + 1;
    };
    constexpr auto dbl = [](int e) {
        return e * 2;
    };
    constexpr auto both = [](int e) {
        return (e + 1) * 2;
    };

    STATIC_REQUIRE((engaged | transform_error(inc) | transform_error(dbl)) == (engaged | transform_error(both)));
    STATIC_REQUIRE((failed | transform_error(inc) | transform_error(dbl)) == (failed | transform_error(both)));
}

} // namespace beman::monadics::tests
