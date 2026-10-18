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

    constexpr auto bundled = [](int a) {
        return f(a) | and_then(g);
    };

    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == (engaged | and_then(bundled)));
    STATIC_REQUIRE((failed | and_then(f) | and_then(g)) == (failed | and_then(bundled)));
    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == Box{22});
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto bundled = [](int a) {
        return f(a) | and_then(g);
    };

    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == (engaged | and_then(bundled)));
    STATIC_REQUIRE((failed | and_then(f) | and_then(g)) == (failed | and_then(bundled)));
    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == Box{22});
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto bundled = [] {
        return f() | and_then(g);
    };

    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == (engaged | and_then(bundled)));
    STATIC_REQUIRE((failed | and_then(f) | and_then(g)) == (failed | and_then(bundled)));
    STATIC_REQUIRE((failed | and_then(f) | and_then(g)) == CURLE_NOT_BUILT_IN);
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto bundled = [](double a) {
        return f(a) | and_then(g);
    };

    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == (engaged | and_then(bundled)));
    STATIC_REQUIRE((failed | and_then(f) | and_then(g)) == (failed | and_then(bundled)));
    STATIC_REQUIRE((engaged | and_then(f) | and_then(g)) == Box{22.0});
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto bundled = [](int a) {
        return f(a) | and_then(g);
    };

    const Box engaged = make_engaged();
    const Box failed = make_failed();

    REQUIRE(holds_the_same_value(engaged | and_then(f) | and_then(g), engaged | and_then(bundled)));
    REQUIRE(holds_the_same_value(failed | and_then(f) | and_then(g), failed | and_then(bundled)));
    REQUIRE(*(engaged | and_then(f) | and_then(g)) == 22);
}

TEST_CASE("raw_ptr") {
    using namespace raw_box;

    STATIC_REQUIRE([] {
        int ten{10};
        int eleven{11};
        int twentytwo{22};
        const auto f = [&](int& a) -> Box {
            return a == 10 ? &eleven : &a;
        };
        const auto g = [&](int& a) -> Box {
            return a == 11 ? &twentytwo : &a;
        };
        const auto bundled = [&](int& a) {
            return f(a) | and_then(g);
        };

        Box engaged = &ten;
        Box failed = nullptr;

        return (engaged | and_then(f) | and_then(g)) == (engaged | and_then(bundled))
            && (failed | and_then(f) | and_then(g)) == (failed | and_then(bundled))
            && *(engaged | and_then(f) | and_then(g)) == 22;
    }());
}

} // namespace beman::monadics::tests
