// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error_type.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::error_type
struct TraitsErrorType {};

template <>
struct box_traits<TraitsErrorType> {
    using error_type = int;
};

// Branch 2: Box::error_type
struct BoxErrorType {
    using error_type = int;
};

// Branch 3: deduce from Traits::error() return type
struct NullaryTraitsError {};

template <>
struct box_traits<NullaryTraitsError> {
    static constexpr int error() noexcept { return -1; }
};

// Traits::error_type takes priority over Box::error_type
struct BothErrorType {
    using error_type = double; // box says double
};

template <>
struct box_traits<BothErrorType> {
    using error_type = int; // traits says int — should win
};

struct NoErrorType {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoErrorType, false),
                       (TraitsErrorType, true),
                       (BoxErrorType, true),
                       (NullaryTraitsError, true),
                       (BothErrorType, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_error_type<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_error_type<Box, Traits>);
    }
}

TEST_CASE("traits-branch") {
    using T = deduce_error_type<TraitsErrorType, box_traits<TraitsErrorType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("box-branch") {
    using T = deduce_error_type<BoxErrorType, box_traits<BoxErrorType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("nullary-traits-error-branch") {
    using T = deduce_error_type<NullaryTraitsError, box_traits<NullaryTraitsError>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

TEST_CASE("traits-wins-over-box") {
    // box says double, traits says int
    using T = deduce_error_type<BothErrorType, box_traits<BothErrorType>>;
    STATIC_REQUIRE(std::same_as<T, int>);
}

} // namespace beman::monadics::detail::tests
