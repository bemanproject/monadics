#include <beman/monadics/detail/deduce_value_type.hpp>

#include <catch2/catch_test_macros.hpp>

#include <optional>

namespace beman::monadics::detail::tests {

template <typename T>
struct Custom {};

TEST_CASE("has-value-type") {
    STATIC_REQUIRE(has_value_type<std::optional<int>, void>);
    STATIC_REQUIRE(has_value_type<void, std::optional<int>>);
    STATIC_REQUIRE(has_value_type<Custom<int>, void>);

    STATIC_REQUIRE(has_value_type<void, Custom<int>> == false);
    STATIC_REQUIRE(has_value_type<int, void> == false);
}

TEST_CASE("deduce") {
    {
        using Box    = std::optional<int>;
        using Traits = void;
        using T      = deduce_value_type<Box, Traits>;
        STATIC_REQUIRE(std::same_as<T, int>);
    }

    {
        using Box    = void;
        using Traits = std::optional<int>;
        using T      = deduce_value_type<Box, Traits>;
        STATIC_REQUIRE(std::same_as<T, int>);
    }

    {
        using Box    = Custom<float>;
        using Traits = std::optional<int>;
        using T      = deduce_value_type<Box, Traits>;
        STATIC_REQUIRE(std::same_as<T, int>);
    }

    {
        using Box    = Custom<int>;
        using Traits = std::optional<float>;
        using T      = deduce_value_type<Box, Traits>;
        STATIC_REQUIRE(std::same_as<T, float>);
    }

    {
        using Box    = Custom<int>;
        using Traits = void;
        using T      = deduce_value_type<Box, Traits>;
        STATIC_REQUIRE(std::same_as<T, int>);
    }
}

} // namespace beman::monadics::detail::tests
