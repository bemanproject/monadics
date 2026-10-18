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

    constexpr auto id = [](int e) {
        return e;
    };

    STATIC_REQUIRE((engaged | transform_error(id)) == engaged);
    STATIC_REQUIRE((failed | transform_error(id)) == failed);
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto id = [](Box e) {
        return e;
    };

    STATIC_REQUIRE((engaged | transform_error(id)) == engaged);
    STATIC_REQUIRE((failed | transform_error(id)) == failed);
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto id = [](int e) {
        return e;
    };

    STATIC_REQUIRE((engaged | transform_error(id)) == engaged);
    STATIC_REQUIRE((failed | transform_error(id)) == failed);
}

} // namespace beman::monadics::tests
