// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_rebind_error.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

// Branch 1: Traits::template rebind_error<E>
template <typename V, typename E>
struct ReboundByTraits {};

struct TraitsRebindError {};

template <>
struct box_traits<TraitsRebindError> {
    template <typename E>
    using rebind_error = ReboundByTraits<int, E>;
};

// Branch 2: Box::template rebind_error<E>
template <typename V, typename E>
struct BoxRebindError {
    template <typename F>
    using rebind_error = BoxRebindError<V, F>;
};

// Branch 3: Traits::error() exists — sentinel no_rebind_error<Box>
struct NullaryErrorBox {};

template <>
struct box_traits<NullaryErrorBox> {
    static constexpr int error() noexcept { return -1; }
};

// Branch 4: deducible second template parameter (meta_rebind_error)
template <typename V, typename E>
struct Expected {};

// Traits::rebind_error takes priority over Box::rebind_error
template <typename V, typename E>
struct BothRebindError {
    template <typename F>
    using rebind_error = BothRebindError<V, F>; // box provides rebind_error
};

template <typename V, typename E>
struct box_traits<BothRebindError<V, E>> {
    template <typename F>
    using rebind_error = ReboundByTraits<V, F>; // traits provides rebind_error — should win
};

struct NoRebindError {};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, bool Has), Box, Has),
                       (int, false),
                       (NoRebindError, false),
                       (TraitsRebindError, true),
                       (BoxRebindError<int, int>, true),
                       (NullaryErrorBox, true),
                       (Expected<int, int>, true),
                       (BothRebindError<int, int>, true)) {
    using Traits = box_traits<Box>;
    STATIC_REQUIRE(has_rebind_error<Box, Traits, double> == Has);
}

TEST_CASE("traits-branch-provider-is-Traits") {
    using Provider = deduce_rebind_error<TraitsRebindError, box_traits<TraitsRebindError>, double>;
    STATIC_REQUIRE(std::same_as<Provider, box_traits<TraitsRebindError>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind_error<double>, ReboundByTraits<int, double>>);
}

TEST_CASE("box-branch-provider-is-Box") {
    using Box      = BoxRebindError<int, int>;
    using Provider = deduce_rebind_error<Box, box_traits<Box>, double>;
    STATIC_REQUIRE(std::same_as<Provider, Box>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind_error<double>, BoxRebindError<int, double>>);
}

TEST_CASE("nullary-error-sentinel-no_rebind_error-maps-any-E-to-Box") {
    using Box      = NullaryErrorBox;
    using Provider = deduce_rebind_error<Box, box_traits<Box>, double>;
    // Sentinel: rebind_error<anything> == Box
    STATIC_REQUIRE(std::same_as<Provider, no_rebind_error<Box>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind_error<double>, Box>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind_error<int>, Box>);
}

// TEST_CASE("meta-branch-provider-is-meta_rebind_error") {
// using Box      = Expected<int, int>;
// using Provider = deduce_rebind_error<Box, box_traits<Box>, double>;
// STATIC_REQUIRE(std::same_as<Provider::template rebind_error<double>, Expected<int, double>>);
// }

TEST_CASE("traits-wins-over-box") {
    using Box      = BothRebindError<int, int>;
    using Provider = deduce_rebind_error<Box, box_traits<Box>, double>;
    STATIC_REQUIRE(std::same_as<Provider, box_traits<Box>>);
    STATIC_REQUIRE(std::same_as<Provider::template rebind_error<double>, ReboundByTraits<int, double>>);
}

} // namespace beman::monadics::detail::tests
