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

    STATIC_REQUIRE((Traits::make(10) | and_then(f)) == f(10));
    STATIC_REQUIRE((Traits::make(10) | and_then(f)) == Box{11});
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    STATIC_REQUIRE((Traits::make(10) | and_then(f)) == f(10));
    STATIC_REQUIRE((Traits::make(10) | and_then(f)) == Box{11});
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    STATIC_REQUIRE((Traits::make() | and_then(f)) == f());
    STATIC_REQUIRE((Traits::make() | and_then(f)) == CURLE_OK);
}

TEST_CASE("result") {
    using namespace result_box;

    STATIC_REQUIRE((Traits::make(10.0) | and_then(f)) == f(10.0));
    STATIC_REQUIRE((Traits::make(10.0) | and_then(f)) == Box{11.0});
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    REQUIRE(holds_the_same_value(Traits::make(10) | and_then(f), f(10)));
    REQUIRE(*(Traits::make(10) | and_then(f)) == 11);
}

TEST_CASE("raw_ptr") {
    using namespace raw_box;

    STATIC_REQUIRE([] {
        int ten{10};
        int eleven{11};
        const auto f = [&](int& a) -> Box {
            return a == 10 ? &eleven : &a;
        };

        return (Traits::make(ten) | and_then(f)) == f(ten);
    }());
}

} // namespace beman::monadics::tests
