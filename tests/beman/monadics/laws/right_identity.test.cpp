// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures/enum.hpp"
#include "fixtures/optional.hpp"
#include "fixtures/raw_ptr.hpp"
#include "fixtures/result.hpp"
#include "fixtures/shared_ptr.hpp"
#if defined(__cpp_lib_expected)
#include "fixtures/expected.hpp"
#endif

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("optional") {
    using namespace opt_box;

    constexpr auto rewrap = [](int a) {
        return Traits::make(a);
    };

    STATIC_REQUIRE((engaged | and_then(rewrap)) == engaged);
    STATIC_REQUIRE((failed | and_then(rewrap)) == failed);
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto rewrap = [](int a) {
        return Traits::make(a);
    };

    STATIC_REQUIRE((engaged | and_then(rewrap)) == engaged);
    STATIC_REQUIRE((failed | and_then(rewrap)) == failed);
    STATIC_REQUIRE((failed | and_then(rewrap)).error() == 7);
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto rewrap = [] {
        return Traits::make();
    };

    STATIC_REQUIRE((engaged | and_then(rewrap)) == engaged);
    STATIC_REQUIRE((failed | and_then(rewrap)) == failed);
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto rewrap = [](double a) {
        return Traits::make(a);
    };

    STATIC_REQUIRE((engaged | and_then(rewrap)) == engaged);
    STATIC_REQUIRE((failed | and_then(rewrap)) == failed);
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto rewrap = [](int a) {
        return Traits::make(a);
    };

    const Box engaged = make_engaged();
    const Box failed = make_failed();

    REQUIRE(holds_the_same_value(engaged | and_then(rewrap), engaged));
    REQUIRE(holds_the_same_value(failed | and_then(rewrap), failed));
}

TEST_CASE("raw_ptr") {
    using namespace raw_box;

    STATIC_REQUIRE([] {
        int ten{10};
        const auto rewrap = [](int& a) {
            return Traits::make(a);
        };

        Box engaged = &ten;
        Box failed = nullptr;

        return (engaged | and_then(rewrap)) == engaged && (failed | and_then(rewrap)) == failed;
    }());
}

} // namespace beman::monadics::tests
