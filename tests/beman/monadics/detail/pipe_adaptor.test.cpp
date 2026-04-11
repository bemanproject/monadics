// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/pipe_adaptor.hpp>

#include <catch2/catch_test_macros.hpp>

#include <helpers/move_tracker.hpp>

namespace beman::monadics::detail::tests {

struct test_op_t {
    inline static constexpr access_key<test_op_t> key{};

    template<typename Box, std::derived_from<test_op_t> Op>
    [[nodiscard]] friend constexpr auto operator|(Box&& box, Op&& op) {
        return std::forward<Op>(op).identity(key)(std::forward<Box>(box));
    }
};

inline constexpr pipe_adaptor<test_op_t> test_op{};

TEST_CASE("with-value") {
    constexpr auto result = 10 | test_op([](auto&& v) { return v * 2; });
    STATIC_REQUIRE(std::same_as<decltype(result), const int>);
    STATIC_REQUIRE(result == 20);
}

TEST_CASE("lvalue-callable-is-copied") {
    struct tracker_fn {
        helpers::MoveTracker tracker;

        constexpr int operator()(int v) const { return v; }
    };

    constexpr auto closure = [] {
        tracker_fn fn{};
        return test_op(fn);
    }();

    STATIC_REQUIRE(closure.identity(test_op_t::key).tracker.copies == 1);
    STATIC_REQUIRE(closure.identity(test_op_t::key).tracker.moves == 0);
}

TEST_CASE("vvalue-callable-is-copied") {
    struct tracker_fn {
        helpers::MoveTracker tracker;

        constexpr int operator()(int v) const { return v; }
    };

    constexpr auto closure = [] {
        tracker_fn fn{};
        return test_op(std::move(fn));
    }();

    STATIC_REQUIRE(closure.identity(test_op_t::key).tracker.copies == 0);
    STATIC_REQUIRE(closure.identity(test_op_t::key).tracker.moves == 1);
}

TEST_CASE("fn-stored-by-value-no-dangling") {
    struct Fn {
        int add{};

        constexpr int operator()(int box) noexcept { return box + this->add; }
    };

    constexpr auto result = [] {
        Fn fn{.add = 15};
        auto closure = test_op(fn);
        return 10 | std::move(closure);
    }();

    STATIC_CHECK(result == 25);
}

} // namespace beman::monadics::detail::tests
