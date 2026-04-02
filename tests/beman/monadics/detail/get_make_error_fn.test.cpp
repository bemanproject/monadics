// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/get_make_error_fn.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <helpers/move_tracker.hpp>

namespace beman::monadics::detail::tests {

namespace {

template <typename T>
struct box_traits {};

struct TraitsMakeError {
    int err{};
};

template <>
struct box_traits<TraitsMakeError> {
    static constexpr auto make_error(int e) noexcept { return TraitsMakeError{e}; }
};

struct TypeConstructibleWithError {
    int err{};
};

struct TraitsAndTypeConstructibleWithError {
    int err{};
};

template <>
struct box_traits<TraitsAndTypeConstructibleWithError> {
    static constexpr TraitsAndTypeConstructibleWithError make_error(int e) noexcept { return {-e}; }
};

struct NonTypeConstructibleWithError {
    struct tag {};

    NonTypeConstructibleWithError() = delete;

    explicit NonTypeConstructibleWithError(struct tag) {}
};

} // namespace

TEMPLATE_TEST_CASE_SIG("concept",
                       "",
                       ((typename Box, typename E, bool Has), Box, E, Has),
                       (TraitsMakeError, int, true),
                       (TypeConstructibleWithError, int, true),
                       (TraitsAndTypeConstructibleWithError, int, true),
                       (NonTypeConstructibleWithError, int, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Has) {
        STATIC_REQUIRE(has_make_error_fn<Box, Traits, E>);
    } else {
        STATIC_REQUIRE_FALSE(has_make_error_fn<Box, Traits, E>);
    }
}

TEMPLATE_TEST_CASE_SIG("get",
                       "",
                       ((typename Box, typename E, int Expected), Box, E, Expected),
                       (TraitsMakeError, int, -1),
                       (TypeConstructibleWithError, int, -1),
                       (TraitsAndTypeConstructibleWithError, int, 1)) {
    using Traits = box_traits<Box>;
    constexpr auto fn = get_make_error_fn<Box, Traits, E>();
    REQUIRE(fn(-1).err == Expected);
}

namespace {

struct MakeErrorTrackerBox {
    helpers::MoveTracker err{};
};

template <>
struct box_traits<MakeErrorTrackerBox> {
    static constexpr decltype(auto) make_error(auto&& e) { return MakeErrorTrackerBox{std::forward<decltype(e)>(e)}; }
};

} // namespace

TEST_CASE("lvalue-arg-is-copied-not-moved") {
    using Box = MakeErrorTrackerBox;
    using Traits = box_traits<Box>;

    constexpr auto result = [] {
        constexpr auto fn = get_make_error_fn<Box, Traits, helpers::MoveTracker>();
        helpers::MoveTracker t;
        return fn(t);
    }();

    STATIC_REQUIRE(result.err.moves == 0);
    STATIC_REQUIRE(result.err.copies == 1);
}

TEST_CASE("rvalue-arg-is-moved-not-copied") {
    constexpr auto result = [] {
        auto fn = get_make_error_fn<MakeErrorTrackerBox, box_traits<MakeErrorTrackerBox>, helpers::MoveTracker>();
        helpers::MoveTracker t;
        return fn(std::move(t));
    }();

    STATIC_REQUIRE(result.err.moves == 1);
    STATIC_REQUIRE(result.err.copies == 0);
}

} // namespace beman::monadics::detail::tests
