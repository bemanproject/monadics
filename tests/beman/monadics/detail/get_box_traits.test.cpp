// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_box_traits.hpp>

#include <catch2/catch_test_macros.hpp>

struct NothingOnEitherSide {
    bool ok{true};
};

struct SaysWhichStateIsWhich {
    bool ok{true};
};

template<typename Box>
struct both_cases_empty {
    using value_type = void;
    using error_type = beman::monadics::detail::unit;

    template<typename V>
    using rebind = Box;

    template<typename E>
    using rebind_error = Box;

    [[nodiscard]] static constexpr bool has_value(const Box& b) noexcept { return b.ok; }

    static constexpr void value(Box&&) noexcept {}
};

template<>
struct beman::monadics::detail::box_traits<NothingOnEitherSide> : both_cases_empty<NothingOnEitherSide> {};

template<>
struct beman::monadics::detail::box_traits<SaysWhichStateIsWhich> : both_cases_empty<SaysWhichStateIsWhich> {
    [[nodiscard]] static constexpr SaysWhichStateIsWhich make_error(unit) noexcept { return {false}; }
};

namespace beman::monadics::detail::tests {

TEST_CASE("a-box-carrying-nothing-on-either-side-must-say-which-state-is-which") {
    STATIC_REQUIRE_FALSE(box<NothingOnEitherSide>);
    STATIC_REQUIRE(box<SaysWhichStateIsWhich>);

    STATIC_REQUIRE(get_box_traits<SaysWhichStateIsWhich>::make().ok);
    STATIC_REQUIRE_FALSE(get_box_traits<SaysWhichStateIsWhich>::make_error(unit{}).ok);
}

} // namespace beman::monadics::detail::tests
