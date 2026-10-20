// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_error.hpp>
#include <beman/monadics/detail/unit.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace beman::monadics::detail::tests {

namespace {

template<typename T>
struct box_traits {};

struct NamesAndReadsAlike {
    int e{7};
};

template<>
struct box_traits<NamesAndReadsAlike> {
    using error_type = int;

    [[nodiscard]] static constexpr int error(const NamesAndReadsAlike& b) noexcept { return b.e; }
};

struct NamesUnitReadsInt {
    int e{7};
};

template<>
struct box_traits<NamesUnitReadsInt> {
    using error_type = unit;

    [[nodiscard]] static constexpr int error(const NamesUnitReadsInt& b) noexcept { return b.e; }
};

struct StoresNothing {
    using error_type = unit;
};

struct NamesATypedFailureWithNoRead {
    using error_type = int;
};

template<typename Box>
concept readable = requires { get_error<Box, box_traits<Box>>(); };

template<typename Box>
concept names_a_type = requires { typename get_error_t<Box, box_traits<Box>>; };

} // namespace

TEST_CASE("one-name-for-the-type-and-the-read") {
    using Box = NamesAndReadsAlike;
    using Traits = box_traits<Box>;

    STATIC_REQUIRE(has_error_access<Box, Traits>);
    STATIC_REQUIRE(std::same_as<get_error_t<Box, Traits>, int>);
    STATIC_REQUIRE(get_error<Box, Traits>()(Box{}) == 7);
}

TEST_CASE("the-read-must-yield-the-type-the-box-names") {
    using Box = NamesUnitReadsInt;

    STATIC_REQUIRE_FALSE(has_error_access<Box, box_traits<Box>>);
}

TEST_CASE("a-failure-storing-nothing-needs-no-read") {
    using Box = StoresNothing;
    using Traits = box_traits<Box>;

    STATIC_REQUIRE(has_error_access<Box, Traits>);
    STATIC_REQUIRE(std::same_as<decltype(get_error<Box, Traits>()(Box{})), unit>);
}

TEST_CASE("a-typed-failure-needs-a-read") {
    using Box = NamesATypedFailureWithNoRead;
    using Traits = box_traits<Box>;

    STATIC_REQUIRE(has_error_type<Box, Traits>);
    STATIC_REQUIRE_FALSE(has_error_fn<Box, Traits, int>);
    STATIC_REQUIRE_FALSE(has_error_access<Box, Traits>);
}

TEST_CASE("the-accessor-refuses-rather-than-answering-void") {
    STATIC_REQUIRE_FALSE(readable<NamesATypedFailureWithNoRead>);
    STATIC_REQUIRE(readable<NamesAndReadsAlike>);
}

TEST_CASE("the-alias-refuses-the-same-boxes-the-accessor-does") {
    STATIC_REQUIRE_FALSE(names_a_type<NamesATypedFailureWithNoRead>);
    STATIC_REQUIRE(names_a_type<NamesAndReadsAlike>);
}

} // namespace beman::monadics::detail::tests
