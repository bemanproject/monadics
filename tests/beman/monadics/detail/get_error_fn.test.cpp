// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <helpers/keeps_return_value_category.hpp>

#include <beman/monadics/detail/unit.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template<typename T>
struct box_traits {};

struct MemberError {
    int ec{1};

    constexpr int& error() & { return ec; }

    constexpr int&& error() && { return std::move(ec); }

    constexpr const int& error() const& { return ec; }

    constexpr const int&& error() const&& { return std::move(ec); }
};

struct TraitsError {
    int ec{1};

    constexpr int& errorCode() & { return ec; }

    constexpr int&& errorCode() && { return std::move(ec); }

    constexpr const int& errorCode() const& { return ec; }

    constexpr const int&& errorCode() const&& { return std::move(ec); }
};

template<>
struct box_traits<TraitsError> {
    static constexpr decltype(auto) error(auto&& b) noexcept { return std::forward<decltype(b)>(b).errorCode(); }
};

struct MemberAndTraitsDisagree {
    int error() noexcept { return 2; }

    int code{1};
};

template<>
struct box_traits<MemberAndTraitsDisagree> {
    static constexpr int error(auto&& box) noexcept { return box.code; }
};

struct NoError {};

struct RvalueOnlyError {
    int ec{1};
};

template<>
struct box_traits<RvalueOnlyError> {
    static constexpr int error(RvalueOnlyError&& b) noexcept { return b.ec; }
};

} // namespace

TEMPLATE_TEST_CASE("concept", "", MemberError, TraitsError, MemberAndTraitsDisagree) {
    using Box = TestType;
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(has_error_fn<Box, Traits, int>);
}

TEST_CASE("a-typed-failure-needs-a-read-it-cannot-be-synthesized") {
    STATIC_REQUIRE_FALSE(has_error_fn<NoError, box_traits<NoError>, int>);
    STATIC_REQUIRE(has_error_fn<NoError, box_traits<NoError>, unit>);
}

TEMPLATE_TEST_CASE("get", "", MemberError, TraitsError, MemberAndTraitsDisagree) {
    using Box = TestType;
    using Traits = box_traits<Box>;
    constexpr auto fn = get_error_fn<Box, Traits, int>();
    STATIC_REQUIRE(fn(Box{}) == 1);
}

TEST_CASE("a-read-the-box-cannot-reach-answers-rather-than-fails") {
    using Traits = box_traits<RvalueOnlyError>;
    constexpr auto fn = get_error_fn<RvalueOnlyError, Traits, int>();

    STATIC_REQUIRE(fn(RvalueOnlyError{}) == 1);
    STATIC_REQUIRE_FALSE(std::invocable<decltype(fn), RvalueOnlyError&>);
}

TEST_CASE("a-failure-storing-nothing-reads-as-unit") {
    constexpr auto fn = get_error_fn<NoError, box_traits<NoError>, unit>();
    STATIC_REQUIRE(std::same_as<decltype(fn(NoError{})), unit>);
}

TEMPLATE_TEST_CASE("keep-value-category", "", TraitsError, MemberError) {
    using Box = TestType;
    using Traits = box_traits<Box>;
    using Fn = decltype(get_error_fn<Box, Traits, int>());
    STATIC_REQUIRE(helpers::keeps_return_value_category<Fn, Box, int>);
}

} // namespace beman::monadics::detail::tests
