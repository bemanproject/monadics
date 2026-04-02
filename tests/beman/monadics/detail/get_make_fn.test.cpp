// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_make_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <helpers/move_tracker.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct TraitsMakeBox {
    int val{};
};

template <>
struct box_traits<TraitsMakeBox> {
    static constexpr TraitsMakeBox make(int v) noexcept { return {v}; }
};

struct BoxWithVoidValue {};

struct TypeConstructibleWithValue {
    int val{};
};

struct TypeAndTraitsConstruibleWithValue {
    int val{};
};

template <>
struct box_traits<TypeAndTraitsConstruibleWithValue> {
    static constexpr TypeAndTraitsConstruibleWithValue make(int v) noexcept { return {-v}; }
};

template <>
struct box_traits<int*> {
    static constexpr const int* make(const int& v) noexcept { return &v; }
};

// Nothing works
struct NonTypeConstructibleWithValue {
    struct tag {};

    NonTypeConstructibleWithValue() = delete;
    explicit NonTypeConstructibleWithValue(tag) {}
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, typename T, bool Has), Box, T, Has),
                       (TraitsMakeBox, int, true),
                       (int*, int, true),
                       (BoxWithVoidValue, void, true),
                       (TypeConstructibleWithValue, int, true),
                       (TypeAndTraitsConstruibleWithValue, int, true),
                       (NonTypeConstructibleWithValue, int, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_make_fn<Box, Traits, T>);
    } else {
        STATIC_REQUIRE_FALSE(has_make_fn<Box, Traits, T>);
    }
}

TEMPLATE_TEST_CASE_SIG("fn",
                       "",
                       ((typename Box, typename T, int Expected), Box, T, Expected),
                       (TraitsMakeBox, int, 42),
                       (BoxWithVoidValue, void, 0), // 0 unused; void branch dispatched below
                       (TypeConstructibleWithValue, int, 42),
                       (TypeAndTraitsConstruibleWithValue, int, -42)) {
    using Traits = box_traits<Box>;
    constexpr auto fn = get_make_fn<Box, Traits, T>();
    if constexpr (std::is_void_v<T>) {
        [[maybe_unused]] Box b = fn();
    } else {
        STATIC_REQUIRE(fn(42).val == Expected);
    }
}

TEST_CASE("pointer-lvalue") {
    using Box = int*;
    using Traits = box_traits<Box>;
    constexpr auto fn = get_make_fn<Box, Traits, int>();
    constexpr int  x = 42;
    STATIC_REQUIRE(fn(x) == &x);
}

namespace {

struct MakeTrackerBox {
    helpers::MoveTracker val{};
};

template <>
struct box_traits<MakeTrackerBox> {
    static constexpr decltype(auto) make(auto&& v) { return MakeTrackerBox{std::forward<decltype(v)>(v)}; }
};

} // namespace

TEST_CASE("lvalue-arg-is-copied-not-moved") {
    constexpr auto result = [] {
        constexpr auto       fn = get_make_fn<MakeTrackerBox, box_traits<MakeTrackerBox>, helpers::MoveTracker>();
        helpers::MoveTracker t;
        return fn(t);
    }();

    STATIC_REQUIRE(result.val.moves == 0);
    STATIC_REQUIRE(result.val.copies == 1);
}

TEST_CASE("rvalue-arg-is-moved-not-copied") {
    constexpr auto result = [] {
        constexpr auto       fn = get_make_fn<MakeTrackerBox, box_traits<MakeTrackerBox>, helpers::MoveTracker>();
        helpers::MoveTracker t;
        return fn(std::move(t));
    }();

    STATIC_REQUIRE(result.val.moves == 1);
    STATIC_REQUIRE(result.val.copies == 0);
}

} // namespace beman::monadics::detail::tests
