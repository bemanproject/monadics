// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct MemberValue {
    int ec{42};

    constexpr int&        value() & { return ec; }
    constexpr int&&       value() && { return std::move(ec); }
    constexpr const int&  value() const& { return ec; }
    constexpr const int&& value() const&& { return std::move(ec); }
};

struct TraitsValue {
    int ec{42};

    constexpr int&        valueCode() & { return ec; }
    constexpr int&&       valueCode() && { return std::move(ec); }
    constexpr const int&  valueCode() const& { return ec; }
    constexpr const int&& valueCode() const&& { return std::move(ec); }
};

template <>
struct box_traits<TraitsValue> {
    static constexpr decltype(auto) value(auto&& b) noexcept { return std::forward<decltype(b)>(b).valueCode(); }
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

TEMPLATE_TEST_CASE("get", "", MemberValue, TraitsValue, MemberAndTraits) {
    using Box    = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(get_value_fn<Box, Traits>()(Box{}) == 42);
}

TEMPLATE_TEST_CASE("keep-value-category", "", TraitsValue, MemberValue) {
    using Box    = TestType;
    using Traits = box_traits<Box>;

    constexpr auto fn = get_value_fn<Box, Traits>();

    STATIC_REQUIRE(requires(Box b) {
        { fn(b) } -> std::same_as<int&>;
    });
    STATIC_REQUIRE(requires(const Box& b) {
        { fn(b) } -> std::same_as<const int&>;
    });
    STATIC_REQUIRE(requires {
        { fn(Box{}) } -> std::same_as<int&&>;
    });
    STATIC_REQUIRE(requires(const Box b) {
        { fn(std::move(b)) } -> std::same_as<const int&&>;
    });
}

} // namespace beman::monadics::detail::tests
