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

    constexpr auto recover = [] {
        return Box{100};
    };

    STATIC_REQUIRE((Traits::make_error(unit{}) | or_else(recover)) == recover());
}

#if defined(__cpp_lib_expected)
TEST_CASE("expected") {
    using namespace expected_box;

    constexpr auto recover = [](int e) {
        return Box{e * 10};
    };

    STATIC_REQUIRE((Traits::make_error(7) | or_else(recover)) == recover(7));
}
#endif

TEST_CASE("enum") {
    using namespace enum_box;

    constexpr auto recover = [](Box) {
        return Box{CURLE_OK};
    };

    STATIC_REQUIRE((Traits::make_error(CURLE_NOT_BUILT_IN) | or_else(recover)) == recover(CURLE_NOT_BUILT_IN));
}

TEST_CASE("result") {
    using namespace result_box;

    constexpr auto recover = [](int e) {
        return Box{e * 10.0};
    };

    STATIC_REQUIRE((Traits::make_error(7) | or_else(recover)) == recover(7));
}

TEST_CASE("shared_ptr") {
    using namespace shared_box;

    const auto recover = [] {
        return std::make_shared<int>(100);
    };

    REQUIRE(holds_the_same_value(Traits::make_error(unit{}) | or_else(recover), recover()));
}

TEST_CASE("raw_ptr") {
    using namespace raw_box;

    STATIC_REQUIRE([] {
        int hundred{100};
        const auto recover = [&] {
            return Traits::make(hundred);
        };

        return (Traits::make_error(unit{}) | or_else(recover)) == recover();
    }());
}

} // namespace beman::monadics::tests
