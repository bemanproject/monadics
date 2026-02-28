// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_rebind.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct TypeAndTraitsWithoutRebind {};

template <typename T>
struct TypeWithRebind {
    template <typename U>
    using rebind = TypeWithRebind<U>;
};

struct TraitsWithRebind {};

template <>
struct box_traits<TraitsWithRebind> {
    template <typename T>
    using rebind = TraitsWithRebind;
};

template <typename T, typename U = void>
struct ExtractValueType {};

template <typename T>
struct TypeAndTraitsWithRebind {
    template <typename U>
    using rebind = std::optional<U>;
};

template <typename T>
struct box_traits<TypeAndTraitsWithRebind<T>> {
    template <typename U>
    using rebind = TypeAndTraitsWithRebind<U>;
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (TypeAndTraitsWithoutRebind, false),
                       (TypeWithRebind<int>, true),
                       (TraitsWithRebind, true),
                       (std::optional<int>, true),
                       (ExtractValueType<int>, true),
                       (TypeAndTraitsWithRebind<int>, true)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_rebind<Box, Traits, double>);
    } else {
        STATIC_REQUIRE_FALSE(has_rebind<Box, Traits, double>);
    }
}

TEMPLATE_TEST_CASE_SIG("get",
                       "",
                       ((typename Box, typename Expected), Box, Expected),
                       (TraitsWithRebind, TraitsWithRebind),
                       (TypeWithRebind<int>, TypeWithRebind<double>),
                       (ExtractValueType<int>, ExtractValueType<double>),
                       (TypeAndTraitsWithRebind<int>, TypeAndTraitsWithRebind<double>)) {
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(std::same_as<typename get_rebind_t<Box, Traits, int>::template rebind<double>, Expected>);
}

} // namespace beman::monadics::detail::tests
