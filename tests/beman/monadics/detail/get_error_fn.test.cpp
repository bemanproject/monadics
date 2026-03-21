// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <helpers/keeps_return_value_category.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct MemberError {
    int ec{1};

    constexpr int&        error() & { return ec; }
    constexpr int&&       error() && { return std::move(ec); }
    constexpr const int&  error() const& { return ec; }
    constexpr const int&& error() const&& { return std::move(ec); }
};

struct TraitsErrorWithoutErrorChannel {};

template <>
struct box_traits<TraitsErrorWithoutErrorChannel> {
    static constexpr int error() noexcept { return 1; }
};

struct TraitsErrorWithErrorChannel {
    int ec{1};

    constexpr int&        errorCode() & { return ec; }
    constexpr int&&       errorCode() && { return std::move(ec); }
    constexpr const int&  errorCode() const& { return ec; }
    constexpr const int&& errorCode() const&& { return std::move(ec); }
};

template <>
struct box_traits<TraitsErrorWithErrorChannel> {
    static constexpr decltype(auto) error(auto&& b) noexcept { return std::forward<decltype(b)>(b).errorCode(); }
};

struct MemberAndTraitsWithoutErrorChannel {
    int error() noexcept { return 2; }
};

template <>
struct box_traits<MemberAndTraitsWithoutErrorChannel> {
    static constexpr int error() noexcept { return 1; }
};

struct MemberAndTraitsWithErrorChannel {
    int error() noexcept { return 2; }
    int code{1};
};

template <>
struct box_traits<MemberAndTraitsWithErrorChannel> {
    static constexpr int error(auto&& box) noexcept { return box.code; }
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (MemberError, true),
                       (TraitsErrorWithoutErrorChannel, true),
                       (TraitsErrorWithErrorChannel, true),
                       (MemberAndTraitsWithoutErrorChannel, true),
                       (MemberAndTraitsWithErrorChannel, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_error_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_error_fn<Box, Traits>);
    }
}

TEMPLATE_TEST_CASE("get",
                   "",
                   MemberError,
                   MemberAndTraitsWithErrorChannel,
                   MemberAndTraitsWithoutErrorChannel,
                   TraitsErrorWithErrorChannel,
                   TraitsErrorWithoutErrorChannel) {
    using Box         = TestType;
    using Traits      = box_traits<Box>;
    constexpr auto fn = get_error_fn<Box, Traits>();
    if constexpr (std::invocable<decltype(fn)>) {
        STATIC_REQUIRE(fn() == 1);
    } else {
        STATIC_REQUIRE(fn(Box{}) == 1);
    }
}

TEMPLATE_TEST_CASE("keep-value-category", "", TraitsErrorWithErrorChannel, MemberError) {
    using Box    = TestType;
    using Traits = box_traits<Box>;
    using Fn     = decltype(get_error_fn<Box, Traits>());
    STATIC_REQUIRE(helpers::keeps_return_value_category<Fn, Box, int>);
}

} // namespace beman::monadics::detail::tests
