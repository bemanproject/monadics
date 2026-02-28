// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_rebind_error.hpp>

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

template <typename T, typename E>
struct TypeAndTraitsWithRebindError {
    template <typename E1>
    using rebind_error = TypeAndTraitsWithRebindError<T, E1>;
};

template <typename T, typename E>
struct TypeWithRebind {
    template <typename E1>
    using rebind_error = TypeWithRebind<T, E1>;
};

template <typename V, typename E>
struct ReboundByTraits {};

struct TraitsWithRebindError {};

template <>
struct box_traits<TraitsWithRebindError> {
    template <typename E>
    using rebind_error = TraitsWithRebindError;
};

struct TypeWithoutErrorChannel {};

template <>
struct box_traits<TypeWithoutErrorChannel> {
    static constexpr int error() noexcept { return -1; }
};

template <typename T, typename E>
struct MetaRebindError {};

template <typename T, typename E>
struct box_traits<TypeAndTraitsWithRebindError<T, E>> {
    template <typename E1>
    using rebind_error = TypeAndTraitsWithRebindError<T, E1>;
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (TypeAndTraitsWithRebindError<int, int>, true),
                       (TypeWithRebind<int, int>, true),
                       (TraitsWithRebindError, true),
                       (TypeWithoutErrorChannel, true),
                       (MetaRebindError<int, int>, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_rebind_error<Box, Traits, double>);
    } else {
        STATIC_REQUIRE_FALSE(has_rebind_error<Box, Traits, double>);
    }
}

TEMPLATE_TEST_CASE_SIG("get",
                       "",
                       ((typename Box, typename Expected), Box, Expected),
                       (TraitsWithRebindError, TraitsWithRebindError),
                       (TypeWithRebind<int, int>, TypeWithRebind<int, double>),
                       (TypeWithoutErrorChannel, TypeWithoutErrorChannel),
                       (MetaRebindError<int, int>, MetaRebindError<int, double>),
                       (TypeAndTraitsWithRebindError<int, int>, TypeAndTraitsWithRebindError<int, double>)) {
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(
        std::same_as<typename get_rebind_error_t<Box, Traits, int>::template rebind_error<double>, Expected>);
}

} // namespace beman::monadics::detail::tests
