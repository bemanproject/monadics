// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/meta_rebind_error.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <optional>

template <typename T, typename E>
struct myexpected {
  using value_type = T;
  using error_type = E;
};

namespace beman::monadics::detail::tests {

TEMPLATE_TEST_CASE("preserves-value-type", "", (myexpected<int, double>), (myexpected<double, float>)) {
    using rebinder = typename decltype(get_meta_rebind_error<TestType>())::type;
    STATIC_REQUIRE(std::same_as<typename rebinder::template rebind_error<float>, myexpected<typename TestType::value_type, float>>);
}

template <typename T>
concept rebind_error_extractable = requires { get_meta_rebind_error<T>(); };

TEST_CASE("requires-minimum-2-template-args") {
    STATIC_REQUIRE_FALSE(rebind_error_extractable<int>);
    STATIC_REQUIRE_FALSE(rebind_error_extractable<std::optional<int>>);
}

} // namespace beman::monadics::detail::tests
