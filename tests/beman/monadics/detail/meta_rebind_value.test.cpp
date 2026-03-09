// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/meta_rebind_value.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <utility>

namespace beman::monadics::detail::tests {

TEST_CASE("single-template-arg") {
    using rebinder = typename decltype(get_meta_rebind<std::optional<int>>())::type;
    STATIC_REQUIRE(std::same_as<rebinder::rebind<double>, std::optional<double>>);
}

TEST_CASE("preserves-extra-args") {
    using rebinder = typename decltype(get_meta_rebind<std::pair<int, double>>())::type;
    STATIC_REQUIRE(std::same_as<rebinder::rebind<float>, std::pair<float, double>>);
}

template <typename T>
concept rebindable = requires { get_meta_rebind<T>(); };

TEST_CASE("requires-minimum-1-template-args") {
    STATIC_REQUIRE_FALSE(rebindable<int>);

    struct Foo{};
    STATIC_REQUIRE_FALSE(rebindable<Foo>);
}

} // namespace beman::monadics::detail::tests
