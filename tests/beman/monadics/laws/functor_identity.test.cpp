// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures/enum.hpp"
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

    constexpr auto id = [](int a) {
        return a;
    };

    STATIC_REQUIRE((engaged | transform(id)) == engaged);
    STATIC_REQUIRE((failed | transform(id)) == failed);
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto id = [](int a) {
        return a;
    };

    STATIC_REQUIRE((engaged | transform(id)) == engaged);
    STATIC_REQUIRE((failed | transform(id)) == failed);
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto id = [] {
    };

    STATIC_REQUIRE((engaged | transform(id)) == engaged);
    STATIC_REQUIRE((failed | transform(id)) == failed);
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto id = [](double a) {
        return a;
    };

    STATIC_REQUIRE((engaged | transform(id)) == engaged);
    STATIC_REQUIRE((failed | transform(id)) == failed);
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto id = [](int a) {
        return a;
    };

    const Box engaged = make_engaged();
    const Box failed = make_failed();

    REQUIRE(holds_the_same_value(engaged | transform(id), engaged));
    REQUIRE(holds_the_same_value(failed | transform(id), failed));
}

} // namespace beman::monadics::tests
