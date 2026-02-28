// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value_type.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct TraitsValueType {};

template <>
struct box_traits<TraitsValueType> {
    using value_type = int;
};

template <typename T>
struct ExtractValueType {};

struct BothValueType {
    using value_type = double;
};

template <>
struct box_traits<BothValueType> {
    using value_type = int;
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (std::optional<int>, true),
                       (TraitsValueType, true),
                       (ExtractValueType<int>, true),
                       (BothValueType, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_value_type<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_value_type<Box, Traits>);
    }
}

TEMPLATE_TEST_CASE("get", "", std::optional<int>, TraitsValueType, ExtractValueType<int>, BothValueType) {
    using Box    = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(std::is_same_v<deduce_value_type<Box, Traits>, int>);
}

} // namespace beman::monadics::detail::tests
