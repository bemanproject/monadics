// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: nullary Traits::error()
struct NullaryTraitsError {};

template <>
struct box_traits<NullaryTraitsError> {
    static constexpr int error() noexcept { return 1; }
};

// Branch 2: unary Traits::error(box)
struct UnaryTraitsError {};

template <>
struct box_traits<UnaryTraitsError> {
    static constexpr int error(const auto&) noexcept { return 2; }
};

// Branch 3: Box::error() member
struct MemberError {
    constexpr int error() const noexcept { return 3; }
};

// Priority: nullary (1) wins over unary (2)
struct NullaryAndUnaryTraitsError {};

template <>
struct box_traits<NullaryAndUnaryTraitsError> {
    static constexpr int error() noexcept { return 1; }
    static constexpr int error(const auto&) noexcept { return 2; }
};

// Priority: unary (2) wins over member (3)
struct UnaryTraitsAndMemberError {
    constexpr int error() const noexcept { return 3; }
};

template <>
struct box_traits<UnaryTraitsAndMemberError> {
    static constexpr int error(const auto&) noexcept { return 2; }
};

struct NoError {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoError, false),
                       (NullaryTraitsError, true),
                       (UnaryTraitsError, true),
                       (MemberError, true),
                       (NullaryAndUnaryTraitsError, true),
                       (UnaryTraitsAndMemberError, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_error_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_error_fn<Box, Traits>);
    }
}

TEST_CASE("nullary-traits-branch") {
    constexpr auto fn = get_error_fn<NullaryTraitsError, box_traits<NullaryTraitsError>>();
    STATIC_REQUIRE(fn() == 1);
}

TEST_CASE("unary-traits-branch") {
    constexpr auto fn = get_error_fn<UnaryTraitsError, box_traits<UnaryTraitsError>>();
    STATIC_REQUIRE(fn(UnaryTraitsError{}) == 2);
}

TEST_CASE("member-branch") {
    constexpr auto fn = get_error_fn<MemberError, box_traits<MemberError>>();
    STATIC_REQUIRE(fn(MemberError{}) == 3);
}

TEST_CASE("nullary-traits-wins-over-unary-traits") {
    constexpr auto fn = get_error_fn<NullaryAndUnaryTraitsError, box_traits<NullaryAndUnaryTraitsError>>();
    // nullary returns 1; unary would return 2
    STATIC_REQUIRE(fn() == 1);
}

TEST_CASE("unary-traits-wins-over-member") {
    constexpr auto fn = get_error_fn<UnaryTraitsAndMemberError, box_traits<UnaryTraitsAndMemberError>>();
    // unary returns 2; member would return 3
    STATIC_REQUIRE(fn(UnaryTraitsAndMemberError{}) == 2);
}

} // namespace beman::monadics::detail::tests
