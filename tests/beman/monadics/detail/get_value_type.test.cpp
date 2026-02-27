// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value_type.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::value_type
struct TraitsValueType {};

template <>
struct box_traits<TraitsValueType> {
    using value_type = int;
};

// Branch 2: Box::value_type
struct BoxValueType {
    using value_type = int;
};

// Branch 3: deducible first template parameter
template <typename T>
struct Wrap {};

// Traits::value_type takes priority over Box::value_type
struct BothValueType {
    using value_type = double; // box says double
};

template <>
struct box_traits<BothValueType> {
    using value_type = int; // traits says int — should win
};

struct NoValueType {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoValueType, false),
                       (TraitsValueType, true),
                       (BoxValueType, true),
                       (Wrap<int>, true),
                       (BothValueType, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_value_type<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_value_type<Box, Traits>);
    }
}

TEST_CASE("traits-branch") {
    using T = deduce_value_type<TraitsValueType, box_traits<TraitsValueType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("box-branch") {
    using T = deduce_value_type<BoxValueType, box_traits<BoxValueType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("meta-extract-branch") {
    using T = deduce_value_type<Wrap<int>, box_traits<Wrap<int>>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("traits-wins-over-box") {
    // box says double, traits says int
    using T = deduce_value_type<BothValueType, box_traits<BothValueType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

} // namespace beman::monadics::detail::tests
