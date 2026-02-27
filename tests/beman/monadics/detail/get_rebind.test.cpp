// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_rebind.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::template rebind<T>
template <typename T>
struct ReboundByTraits {};

struct TraitsRebind {};

template <>
struct box_traits<TraitsRebind> {
    template <typename T>
    using rebind = ReboundByTraits<T>;
};

// Branch 2: Box::template rebind<T>
template <typename T>
struct BoxRebind {
    template <typename U>
    using rebind = BoxRebind<U>;
};

// Branch 3: deducible first template parameter (meta_rebind)
template <typename T>
struct Wrap {};

// Traits::rebind takes priority over Box::rebind
template <typename T>
struct BothRebind {
    template <typename U>
    using rebind = BothRebind<U>; // box provides rebind
};

template <typename T>
struct box_traits<BothRebind<T>> {
    template <typename U>
    using rebind = ReboundByTraits<U>; // traits provides rebind — should win
};

struct NoRebind {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoRebind, false),
                       (TraitsRebind, true),
                       (BoxRebind<int>, true),
                       (Wrap<int>, true),
                       (BothRebind<int>, true)) {
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(has_rebind<Box, Traits, double> == Has);
}

TEST_CASE("traits-branch-provider-is-Traits") {
    using Provider = deduce_rebind<TraitsRebind, box_traits<TraitsRebind>, double>;
    STATIC_REQUIRE(std::same_as<Provider, box_traits<TraitsRebind>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind<double>, ReboundByTraits<double>>);
}

TEST_CASE("box-branch-provider-is-Box") {
    using Provider = deduce_rebind<BoxRebind<int>, box_traits<BoxRebind<int>>, double>;
    STATIC_REQUIRE(std::same_as<Provider, BoxRebind<int>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind<double>, BoxRebind<double>>);
}

TEST_CASE("meta-branch-provider-is-meta_rebind") {
    using Provider = deduce_rebind<Wrap<int>, box_traits<Wrap<int>>, double>;
    STATIC_REQUIRE(std::same_as<Provider::template rebind<double>, Wrap<double>>);
}

TEST_CASE("traits-wins-over-box") {
    using Provider = deduce_rebind<BothRebind<int>, box_traits<BothRebind<int>>, double>;
    STATIC_REQUIRE(std::same_as<Provider, box_traits<BothRebind<int>>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind<double>, ReboundByTraits<double>>);
}

} // namespace beman::monadics::detail::tests
