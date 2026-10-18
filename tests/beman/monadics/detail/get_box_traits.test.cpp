// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/just_error.hpp>

#include <catch2/catch_test_macros.hpp>

#include <concepts>

struct NothingOnEitherSide {
    bool ok{true};
};

struct SaysWhichStateIsWhich {
    bool ok{true};
};

template<typename Box>
struct both_cases_empty {
    using value_type = void;
    using error_type = beman::monadics::detail::unit;

    template<typename V>
    using rebind = Box;

    template<typename E>
    using rebind_error = Box;

    [[nodiscard]] static constexpr bool has_value(const Box& b) noexcept { return b.ok; }

    static constexpr void value(Box&&) noexcept {}
};

template<>
struct beman::monadics::detail::box_traits<NothingOnEitherSide> : both_cases_empty<NothingOnEitherSide> {};

template<>
struct beman::monadics::detail::box_traits<SaysWhichStateIsWhich> : both_cases_empty<SaysWhichStateIsWhich> {
    [[nodiscard]] static constexpr SaysWhichStateIsWhich make_error(unit) noexcept { return {false}; }
};

struct NamesUnitReadsInt {
    int e{9};
};

template<>
struct beman::monadics::detail::box_traits<NamesUnitReadsInt> {
    using value_type = int;
    using error_type = unit;

    template<typename V>
    using rebind = NamesUnitReadsInt;

    template<std::same_as<unit> E>
    using rebind_error = NamesUnitReadsInt;

    [[nodiscard]] static constexpr bool has_value(const NamesUnitReadsInt&) noexcept { return false; }

    [[nodiscard]] static constexpr int value(const NamesUnitReadsInt&) noexcept { return 1; }

    [[nodiscard]] static constexpr int error(const NamesUnitReadsInt& b) noexcept { return b.e; }

    [[nodiscard]] static constexpr auto make(int) noexcept { return NamesUnitReadsInt{}; }
};

struct NamesAReference {
    int* p{};
};

template<>
struct beman::monadics::detail::box_traits<NamesAReference> {
    using value_type = int&;
    using error_type = unit;

    template<typename U>
    using rebind = NamesAReference;

    template<std::same_as<unit> E>
    using rebind_error = NamesAReference;

    [[nodiscard]] static constexpr bool has_value(const NamesAReference& b) noexcept { return b.p != nullptr; }

    [[nodiscard]] static constexpr int& value(const NamesAReference& b) noexcept { return *b.p; }

    [[nodiscard]] static constexpr NamesAReference make(int& v) noexcept { return {&v}; }
};

struct PromisesAReferenceReadsAValue {
    int v{};
};

template<>
struct beman::monadics::detail::box_traits<PromisesAReferenceReadsAValue> {
    using value_type = int&;
    using error_type = unit;

    template<typename U>
    using rebind = PromisesAReferenceReadsAValue;

    template<std::same_as<unit> E>
    using rebind_error = PromisesAReferenceReadsAValue;

    [[nodiscard]] static constexpr bool has_value(const PromisesAReferenceReadsAValue&) noexcept { return true; }

    [[nodiscard]] static constexpr int value(const PromisesAReferenceReadsAValue& b) noexcept { return b.v; }

    [[nodiscard]] static constexpr PromisesAReferenceReadsAValue make(int& v) noexcept { return {v}; }
};

struct NamesNeverAndSuppliesIt {
    int e{7};
};

template<>
struct beman::monadics::detail::box_traits<NamesNeverAndSuppliesIt> {
    using value_type = int;
    using error_type = never;

    template<typename V>
    using rebind = NamesNeverAndSuppliesIt;

    template<typename E>
    using rebind_error = NamesNeverAndSuppliesIt;

    [[nodiscard]] static constexpr bool has_value(const NamesNeverAndSuppliesIt&) noexcept { return true; }

    [[nodiscard]] static constexpr int value(const NamesNeverAndSuppliesIt&) noexcept { return 1; }

    [[nodiscard]] static constexpr auto make(int) noexcept { return NamesNeverAndSuppliesIt{}; }

    [[nodiscard]] static constexpr int error(const NamesNeverAndSuppliesIt& b) noexcept { return b.e; }
};

namespace beman::monadics::detail::tests {

TEST_CASE("a-box-carrying-nothing-on-either-side-must-say-which-state-is-which") {
    STATIC_REQUIRE_FALSE(box<NothingOnEitherSide>);
    STATIC_REQUIRE(box<SaysWhichStateIsWhich>);

    STATIC_REQUIRE(get_box_traits<SaysWhichStateIsWhich>::make().ok);
    STATIC_REQUIRE_FALSE(get_box_traits<SaysWhichStateIsWhich>::make_error(unit{}).ok);
}

TEST_CASE("a-read-must-yield-the-case-type-the-box-names") {
    STATIC_REQUIRE_FALSE(box<NamesUnitReadsInt>);
}

TEST_CASE("a-side-named-never-must-not-also-be-supplied") {
    STATIC_REQUIRE_FALSE(box<NamesNeverAndSuppliesIt>);
}

TEST_CASE("a-case-type-may-be-a-reference-if-the-read-is-one-too") {
    STATIC_REQUIRE(box<NamesAReference>);
    STATIC_REQUIRE(std::same_as<get_box_traits<NamesAReference>::value_type, int&>);
}

TEST_CASE("promising-a-reference-and-reading-a-value-is-refused") {
    STATIC_REQUIRE_FALSE(box<PromisesAReferenceReadsAValue>);
}

TEST_CASE("a-box-with-neither-case-is-neither-value-only-nor-error-only") {
    using Dead = just_error_t<never>;

    STATIC_REQUIRE(no_case<Dead>);
    STATIC_REQUIRE_FALSE(value_only<Dead>);
    STATIC_REQUIRE_FALSE(error_only<Dead>);
    STATIC_REQUIRE_FALSE(single_case<Dead>);

    STATIC_REQUIRE(no_value_case<just_error_t<int>>);
    STATIC_REQUIRE_FALSE(no_case<just_error_t<int>>);
    STATIC_REQUIRE(single_case<just_error_t<int>>);
}

} // namespace beman::monadics::detail::tests
