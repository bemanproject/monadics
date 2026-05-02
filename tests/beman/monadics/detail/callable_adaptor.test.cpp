// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/callable_adaptor.hpp>

#include <catch2/catch_test_macros.hpp>

#include <helpers/move_tracker.hpp>

namespace beman::monadics::detail::tests {

template<typename Fn>
struct test_op_t {
    constexpr explicit test_op_t(Fn fn) : fn_(std::move(fn)) {}

    friend constexpr decltype(auto) operator|(auto&& box, test_op_t&& op) {
        return std::move(op.fn_)(std::forward<decltype(box)>(box));
    }

  private:
    Fn fn_;
};

inline constexpr callable_adaptor<test_op_t> test_op{};

TEST_CASE("with-value") {
    constexpr auto result = 10 | test_op([](auto&& v) { return v * 2; });
    STATIC_REQUIRE(std::same_as<decltype(result), const int>);
    STATIC_REQUIRE(result == 20);
}

TEST_CASE("lvalue-callable-is-copied") {
    struct tracker_fn {
        helpers::MoveTracker tracker;
        constexpr int operator()(int) const { return tracker.copies; }
    };

    constexpr auto result = [] {
        tracker_fn fn{};
        return 10 | test_op(fn);
    }();

    STATIC_REQUIRE(result == 1);
}

TEST_CASE("rvalue-callable-is-moved") {
    struct tracker_fn {
        helpers::MoveTracker tracker;
        constexpr int operator()(int) const { return tracker.moves; }
    };

    constexpr auto result = [] {
        tracker_fn fn{};
        return 10 | test_op(std::move(fn));
    }();

    STATIC_REQUIRE(result == 2);
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
