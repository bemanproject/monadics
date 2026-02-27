// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct MemberValue {
    constexpr int value() const noexcept { return 42; }
};

struct TraitsValue {};

template <>
struct box_traits<TraitsValue> {
    static constexpr int value(const auto&) noexcept { return 42; }
};

// Member returns 0; traits returns 42 — makes priority test load-bearing.
struct MemberAndTraits {
    constexpr int value() const noexcept { return 0; }
};

template <>
struct box_traits<MemberAndTraits> {
    static constexpr int value(const auto&) noexcept { return 42; }
};

struct NoValue {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoValue, false),
                       (MemberValue, true),
                       (TraitsValue, true),
                       (MemberAndTraits, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_value_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_value_fn<Box, Traits>);
    }
}

TEMPLATE_TEST_CASE("get", "", MemberValue, TraitsValue) {
    using Box    = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(get_value_fn<Box, Traits>()(Box{}) == 42);
}

TEST_CASE("traits-wins-over-member-when-both-present") {
    using Box         = MemberAndTraits;
    using Traits      = box_traits<Box>;
    constexpr auto fn = get_value_fn<Box, Traits>();
    // member returns 0; if traits wins: fn(box) == 42
    STATIC_REQUIRE(fn(Box{}) == 42);
}

} // namespace beman::monadics::detail::tests
