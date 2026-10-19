// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/just.hpp>
#include <beman/monadics/monadics.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <type_traits>
#include <utility>

template<typename T, typename E>
struct Fallible {
    struct fail {
        E error;
    };

    bool ok{true};
    T v{};
    E e{};

    constexpr explicit(false) Fallible(T value) : v(std::move(value)) {}

    constexpr explicit Fallible(fail f) : ok(false), e(std::move(f.error)) {}

    [[nodiscard]] constexpr bool operator==(const Fallible&) const = default;
};

template<typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    using error_type = unit;
};

template<typename T, typename E>
struct beman::monadics::box_traits<Fallible<T, E>> {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = Fallible<U, E>;

    template<typename F>
    using rebind_error = Fallible<T, F>;

    [[nodiscard]] static constexpr bool has_value(const Fallible<T, E>& b) noexcept { return b.ok; }

    [[nodiscard]] static constexpr decltype(auto) value(auto&& b) { return std::forward<decltype(b)>(b).v; }

    [[nodiscard]] static constexpr decltype(auto) error(auto&& b) { return std::forward<decltype(b)>(b).e; }

    [[nodiscard]] static constexpr auto make(auto&& v) { return Fallible<T, E>{std::forward<decltype(v)>(v)}; }

    [[nodiscard]] static constexpr auto make_error(auto&& e) {
        return Fallible<T, E>{typename Fallible<T, E>::fail{std::forward<decltype(e)>(e)}};
    }
};

namespace beman::monadics::tests {

namespace {

using O = std::optional<int>;
using X = Fallible<int, int>;

inline constexpr auto pass_value = [](int v) {
    return v;
};
inline constexpr auto want_rvalue = [](int&&) {
    return 0;
};
inline constexpr auto want_lvalue = [](int&) {
    return 0;
};
inline constexpr auto ret_int = [](auto&&) {
    return 1;
};
inline constexpr auto ret_double = [](auto&&) {
    return 2.0;
};
inline constexpr auto any = [](auto&&) {
    return 0;
};

inline constexpr auto told = []<typename R>([[maybe_unused]] auto&&... v) -> R {
    return R{static_cast<int>(sizeof...(v))};
};

template<typename OnValue, typename OnError, typename Box>
concept either_call =
    requires { detail::either(std::declval<OnValue>(), std::declval<OnError>(), std::declval<Box>()); };

} // namespace

TEST_CASE("dispatches-to-the-inhabited-case") {
    STATIC_REQUIRE(detail::either(pass_value, [](unit) { return -1; }, O{5}) == 5);
    STATIC_REQUIRE(detail::either(pass_value, [](unit) { return -1; }, O{}) == -1);
    STATIC_REQUIRE(detail::either(pass_value, [](int e) { return e; }, X{5}) == 5);
    STATIC_REQUIRE(detail::either(pass_value, [](int e) { return e; }, X{X::fail{7}}) == 7);
}

TEST_CASE("failure-carrying-nothing-accepts-a-nullary-handler") {
    STATIC_REQUIRE(detail::either(pass_value, [] { return -1; }, O{}) == -1);
}

TEST_CASE("callable-more-than-once") {
    constexpr O a{1};
    constexpr O b{2};
    STATIC_REQUIRE(detail::either(pass_value, any, a) + detail::either(pass_value, any, b) == 3);
}

TEMPLATE_TEST_CASE_SIG("keep-value-category",
                       "",
                       ((typename Box, auto OnValue, bool Expected), Box, OnValue, Expected),
                       (O&, want_lvalue, true),
                       (O&, want_rvalue, false),
                       (O&&, want_rvalue, true),
                       (O&&, want_lvalue, false),
                       (const O&, want_lvalue, false)) {
    STATIC_REQUIRE(either_call<decltype(OnValue), decltype(any), Box> == Expected);
}

TEST_CASE("handlers-may-return-nothing") {
    constexpr auto seen = [](auto&& box) {
        int n = 0;
        detail::either([&](int v) { n = v; }, [&] { n = -1; }, box);
        return n;
    };

    STATIC_REQUIRE(seen(O{10}) == 10);
    STATIC_REQUIRE(seen(O{}) == -1);
    STATIC_REQUIRE(std::is_void_v<decltype(detail::either([](int) {}, [] {}, O{1}))>);
}

TEST_CASE("handlers-must-agree-on-the-return-type") {
    STATIC_REQUIRE(either_call<decltype(ret_int), decltype(ret_int), O&>);
    STATIC_REQUIRE_FALSE(either_call<decltype(ret_int), decltype(ret_double), O&>);
}

TEST_CASE("a-handler-result-is-never-asked-to-be-a-box") {
    STATIC_REQUIRE_FALSE(box<int>);
    STATIC_REQUIRE(std::same_as<decltype(detail::either(ret_int, ret_int, O{1})), int>);
    STATIC_REQUIRE(detail::either(ret_int, ret_int, O{1}) == 1);
}

TEST_CASE("a-carrier-is-not-special-the-handlers-must-still-agree") {
    STATIC_REQUIRE_FALSE(either_call<decltype(pass_value), detail::just_error_fn, X&>);
    STATIC_REQUIRE_FALSE(either_call<detail::just_fn, detail::just_error_fn, X&>);
}

TEST_CASE("tells-the-result-to-the-handler-that-does-not-name-one") {
    STATIC_REQUIRE(detail::either(pass_value, told, O{5}) == 5);
    STATIC_REQUIRE(detail::either(pass_value, told, O{}) == 1);
    STATIC_REQUIRE(detail::either(told, [](unit) { return 7; }, O{}) == 7);
    STATIC_REQUIRE(detail::either(told, [](unit) { return 7; }, O{5}) == 1);
}

TEST_CASE("a-told-handler-takes-nothing-where-the-value-carries-nothing") {
    STATIC_REQUIRE(detail::either(told, any, detail::just_t<void>{}) == 0);
}

TEST_CASE("neither-handler-naming-a-result-is-not-enough") {
    STATIC_REQUIRE_FALSE(either_call<decltype(told), decltype(told), O&>);
}
TEST_CASE("rejects-a-non-box") {
    STATIC_REQUIRE_FALSE(either_call<decltype(any), decltype(any), int&>);
}

} // namespace beman::monadics::tests
