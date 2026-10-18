// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_value.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template<typename T>
struct box_traits {};

struct NamesAndReadsAlike {
    int v{1};
};

template<>
struct box_traits<NamesAndReadsAlike> {
    using value_type = int;

    [[nodiscard]] static constexpr int value(const NamesAndReadsAlike& b) noexcept { return b.v; }
};

struct PromisesAReferenceReadsAValue {
    int v{1};
};

template<>
struct box_traits<PromisesAReferenceReadsAValue> {
    using value_type = int&;

    [[nodiscard]] static constexpr int value(const PromisesAReferenceReadsAValue& b) noexcept { return b.v; }
};

struct NamesNothingAtAll {};

struct NamesATypeWithNoRead {
    using value_type = int;
};

template<typename Box>
concept readable = requires { get_value<Box, box_traits<Box>>(); };

template<typename Box>
concept names_a_type = requires { typename get_value_t<Box, box_traits<Box>>; };

} // namespace

TEST_CASE("one-name-for-the-type-and-the-read") {
    using Box = NamesAndReadsAlike;
    using Traits = box_traits<Box>;

    STATIC_REQUIRE(has_value_access<Box, Traits>);
    STATIC_REQUIRE(std::same_as<get_value_t<Box, Traits>, int>);
    STATIC_REQUIRE(get_value<Box, Traits>()(Box{}) == 1);
}

TEST_CASE("the-read-must-yield-the-type-the-box-names") {
    using Box = PromisesAReferenceReadsAValue;

    STATIC_REQUIRE_FALSE(has_value_access<Box, box_traits<Box>>);
}

TEST_CASE("each-half-is-still-required-on-its-own") {
    STATIC_REQUIRE_FALSE(has_value_type<NamesNothingAtAll, box_traits<NamesNothingAtAll>>);
    STATIC_REQUIRE_FALSE(has_value_access<NamesNothingAtAll, box_traits<NamesNothingAtAll>>);

    using Named = NamesATypeWithNoRead;
    STATIC_REQUIRE(has_value_type<Named, box_traits<Named>>);
    STATIC_REQUIRE_FALSE(has_value_fn<Named, box_traits<Named>, int>);
    STATIC_REQUIRE_FALSE(has_value_access<Named, box_traits<Named>>);
}

TEST_CASE("the-accessor-refuses-rather-than-answering-void") {
    STATIC_REQUIRE_FALSE(readable<NamesATypeWithNoRead>);
    STATIC_REQUIRE(readable<NamesAndReadsAlike>);
}

TEST_CASE("the-alias-refuses-the-same-boxes-the-accessor-does") {
    STATIC_REQUIRE_FALSE(names_a_type<NamesATypeWithNoRead>);
    STATIC_REQUIRE(names_a_type<NamesAndReadsAlike>);
}

} // namespace beman::monadics::detail::tests
