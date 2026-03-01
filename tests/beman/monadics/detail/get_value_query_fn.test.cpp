// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value_query_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct MemberHasValue {
    bool           valid{};
    constexpr bool has_value() const noexcept { return valid; }
};

struct TraitsHasValue {
    bool valid{};
};

template <>
struct box_traits<TraitsHasValue> {
    static constexpr bool has_value(const auto& box) noexcept { return box.valid; }
};

struct MemberAndTraits {
    bool valid{};
    bool value_flag{};

    bool has_value() const noexcept { return value_flag; }
};

template <>
struct box_traits<MemberAndTraits> {
    static constexpr bool has_value(const auto& box) noexcept { return box.valid; }
};

struct MemberHasValueMissingNoexcept {
    bool has_value() const { return true; }
};

struct MemberHasValueMissingConst {
    bool has_value() { return true; }
};

struct MemberHasValueWrongReturnType {
    constexpr int has_value() const noexcept { return 1; }
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (MemberHasValue, true),
                       (TraitsHasValue, true),
                       (MemberAndTraits, true),
                       (MemberHasValueMissingNoexcept, false),
                       (MemberHasValueMissingConst, false),
                       (MemberHasValueWrongReturnType, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_value_query_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_value_query_fn<Box, Traits>);
    }
}

TEMPLATE_TEST_CASE("get", "", MemberHasValue, TraitsHasValue, MemberAndTraits) {
    using Box    = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(get_value_query_fn<Box, Traits>()(Box{}) == false);
    STATIC_REQUIRE(get_value_query_fn<Box, Traits>()(Box{true}) == true);
}

TEST_CASE("traits-over-member-function") {
    using Box    = MemberAndTraits;
    using Traits = box_traits<Box>;

    STATIC_REQUIRE(get_value_query_fn<Box, Traits>()(Box{.valid = true, .value_flag = false}) == true);
    STATIC_REQUIRE(get_value_query_fn<Box, Traits>()(Box{.valid = false, .value_flag = true}) == false);
}

} // namespace beman::monadics::detail::tests
