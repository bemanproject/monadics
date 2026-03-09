// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/meta_extract_value_type.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <optional>
#include <vector>

namespace beman::monadics::detail::tests {

TEMPLATE_TEST_CASE("get", "", std::optional<int>, std::vector<int>) {
    STATIC_REQUIRE(std::same_as<decltype(meta_extract_value_type<TestType>()),
                                std::type_identity<typename TestType::value_type> >);
}

template <typename T>
concept extractable = requires {
    { meta_extract_value_type<T>() };
};

TEST_CASE("not-callable-for-non-decomposable") {
  STATIC_REQUIRE_FALSE(extractable<int>);

  struct Foo {};
  STATIC_REQUIRE_FALSE(extractable<Foo>);
}

} // namespace beman::monadics::detail::tests
