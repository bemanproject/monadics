// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <beman/monadics/detail/has_box_traits.hpp>

namespace beman::monadics::detail::tests {

namespace {

template<typename T>
struct box_traits {};

// ── has_valid_has_value_fn fixtures ──────────────────────────────────────────

struct HasValueCorrect {};
template<>
struct box_traits<HasValueCorrect> {
    using value_type = int;
    static bool has_value(const HasValueCorrect&) noexcept { return true; }
};

struct HasValueBadReturn {};
struct NoBool {};
template<>
struct box_traits<HasValueBadReturn> {
    using value_type = int;
    static NoBool has_value(const HasValueBadReturn&) noexcept { return {}; }
};

struct HasValueNotNoexcept {};
template<>
struct box_traits<HasValueNotNoexcept> {
    using value_type = int;
    static bool has_value(const HasValueNotNoexcept&) { return true; } // missing noexcept
};

// ── has_valid_value_fn fixtures ───────────────────────────────────────────────

struct ValueCorrect {
    int val{};
};
template<>
struct box_traits<ValueCorrect> {
    using value_type = int;
    static int value(ValueCorrect&&) { return 0; }
};

struct ValueWrongReturn {
    int val{};
};
template<>
struct box_traits<ValueWrongReturn> {
    using value_type = int;
    static double value(ValueWrongReturn&&) { return 3.14; }
};

// ── has_valid_error_fn fixtures ───────────────────────────────────────────────

struct ErrorNullaryCorrect {};
template<>
struct box_traits<ErrorNullaryCorrect> {
    using error_type = int;
    static int error() { return 0; }
};

struct ErrorUnaryCorrect {
    int err{};
};
template<>
struct box_traits<ErrorUnaryCorrect> {
    using error_type = int;
    static int error(ErrorUnaryCorrect&& b) { return b.err; }
};

struct ErrorWrongReturn {};
template<>
struct box_traits<ErrorWrongReturn> {
    using error_type = int;
    static double error(ErrorWrongReturn&&) { return 3.14; }
};

// ── has_valid_make_fn fixtures ────────────────────────────────────────────────

struct MakeCorrect {
    int val{};
};
template<>
struct box_traits<MakeCorrect> {
    using value_type = int;
    static MakeCorrect make(int v) { return {v}; }
};

struct OtherBox {
    int val{};
};

struct MakeWrongReturn {
    int val{};
};
template<>
struct box_traits<MakeWrongReturn> {
    using value_type = int;
    static OtherBox make(int v) { return {v}; }
};

// ── has_valid_make_error_fn fixtures ─────────────────────────────────────────

struct MakeErrorCorrect {
    int err{};
};
template<>
struct box_traits<MakeErrorCorrect> {
    using error_type = int;
    static MakeErrorCorrect make_error(int e) { return {e}; }
};

struct MakeErrorWrongReturn {
    int err{};
};
template<>
struct box_traits<MakeErrorWrongReturn> {
    using error_type = int;
    static OtherBox make_error(int e) { return {e}; }
};

// ── rebind / rebind_error fixtures ────────────────────────────────────────────

template<typename T, typename E>
struct ExpectedLike {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = ExpectedLike<U, E>;

    template<typename F>
    using rebind_error = ExpectedLike<T, F>;
};

template<typename T>
struct OptionalLike {
    using value_type = T;
};

template<typename T, typename E>
struct box_traits<ExpectedLike<T, E>> {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = ExpectedLike<U, E>;

    template<typename F>
    using rebind_error = ExpectedLike<T, F>;

    static bool has_value(const ExpectedLike<T, E>&) noexcept { return true; }
    static T value(ExpectedLike<T, E>&&) { return {}; }
    static E error(ExpectedLike<T, E>&&) { return {}; }
    static ExpectedLike<T, E> make(T) { return {}; }
    static ExpectedLike<T, E> make_error(E) { return {}; }
};

// rebind goes to the wrong template family
template<typename T, typename E>
struct BadRebindTraits {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = OptionalLike<U>; // wrong template family

    template<typename F>
    using rebind_error = ExpectedLike<T, F>;
};

// rebind preserves the right shape but hard-codes the wrong error type
template<typename T, typename E>
struct WrongErrorRebindTraits {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = ExpectedLike<U, int>; // error always becomes int

    template<typename F>
    using rebind_error = ExpectedLike<T, F>;
};

// rebind_error goes to the wrong template family
template<typename T, typename E>
struct BadRebindErrorTraits {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = ExpectedLike<U, E>;

    template<typename F>
    using rebind_error = OptionalLike<F>; // wrong template family
};

// rebind_error preserves the right shape but hard-codes the wrong value type
template<typename T, typename E>
struct WrongValueRebindErrorTraits {
    using value_type = T;
    using error_type = E;

    template<typename U>
    using rebind = ExpectedLike<U, E>;

    template<typename F>
    using rebind_error = ExpectedLike<double, F>; // value always becomes double
};

} // namespace

// ── has_valid_has_value_fn ────────────────────────────────────────────────────

TEMPLATE_TEST_CASE_SIG("has_valid_has_value_fn",
                       "",
                       ((typename Box, bool Valid), Box, Valid),
                       (int, true),                // no has_value -> passes
                       (HasValueCorrect, true),    // correct signature
                       (HasValueBadReturn, false), // non-bool-convertible return
                       (HasValueNotNoexcept, false)) {
    using Traits = box_traits<Box>;
    if constexpr (Valid) {
        STATIC_REQUIRE(has_valid_has_value_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_valid_has_value_fn<Box, Traits>);
    }
}

// ── has_valid_value_fn ────────────────────────────────────────────────────────

TEMPLATE_TEST_CASE_SIG("has_valid_value_fn",
                       "",
                       ((typename Box, bool Valid), Box, Valid),
                       (int, true),               // no value -> passes
                       (ValueCorrect, true),      // correct return type
                       (ValueWrongReturn, false)) // returns double instead of int
{
    using Traits = box_traits<Box>;
    if constexpr (Valid) {
        STATIC_REQUIRE(has_valid_value_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_valid_value_fn<Box, Traits>);
    }
}

// ── has_valid_error_fn ────────────────────────────────────────────────────────

TEMPLATE_TEST_CASE_SIG("has_valid_error_fn",
                       "",
                       ((typename Box, bool Valid), Box, Valid),
                       (int, true),                 // no error -> passes
                       (ErrorNullaryCorrect, true), // nullary, correct return
                       (ErrorUnaryCorrect, true),   // unary, correct return
                       (ErrorWrongReturn, false))   // unary, wrong return type
{
    using Traits = box_traits<Box>;
    if constexpr (Valid) {
        STATIC_REQUIRE(has_valid_error_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_valid_error_fn<Box, Traits>);
    }
}

// ── has_valid_make_fn ─────────────────────────────────────────────────────────

TEMPLATE_TEST_CASE_SIG("has_valid_make_fn",
                       "",
                       ((typename Box, bool Valid), Box, Valid),
                       (int, true),              // no make -> passes
                       (MakeCorrect, true),      // returns Box
                       (MakeWrongReturn, false)) // returns OtherBox
{
    using Traits = box_traits<Box>;
    if constexpr (Valid) {
        STATIC_REQUIRE(has_valid_make_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_valid_make_fn<Box, Traits>);
    }
}

// ── has_valid_make_error_fn ───────────────────────────────────────────────────

TEMPLATE_TEST_CASE_SIG("has_valid_make_error_fn",
                       "",
                       ((typename Box, bool Valid), Box, Valid),
                       (int, true),                   // no make_error -> passes
                       (MakeErrorCorrect, true),      // returns Box
                       (MakeErrorWrongReturn, false)) // returns OtherBox
{
    using Traits = box_traits<Box>;
    if constexpr (Valid) {
        STATIC_REQUIRE(has_valid_make_error_fn<Box, Traits>);
    } else {
        STATIC_REQUIRE_FALSE(has_valid_make_error_fn<Box, Traits>);
    }
}

// ── has_valid_rebind ──────────────────────────────────────────────────────────

using ExpectedIntDouble = ExpectedLike<int, double>;

TEST_CASE("has_valid_rebind - correct traits") {
    STATIC_REQUIRE(has_valid_rebind<ExpectedIntDouble, box_traits<ExpectedIntDouble>>);
}

TEST_CASE("has_valid_rebind - rebind returns wrong template family") {
    STATIC_REQUIRE_FALSE(has_valid_rebind<ExpectedIntDouble, BadRebindTraits<int, double>>);
}

TEST_CASE("has_valid_rebind - rebind corrupts error type") {
    STATIC_REQUIRE_FALSE(has_valid_rebind<ExpectedIntDouble, WrongErrorRebindTraits<int, double>>);
}

// ── has_valid_rebind_error ────────────────────────────────────────────────────

TEST_CASE("has_valid_rebind_error - correct traits") {
    STATIC_REQUIRE(has_valid_rebind_error<ExpectedIntDouble, box_traits<ExpectedIntDouble>>);
}

TEST_CASE("has_valid_rebind_error - rebind_error returns wrong template family") {
    STATIC_REQUIRE_FALSE(has_valid_rebind_error<ExpectedIntDouble, BadRebindErrorTraits<int, double>>);
}

TEST_CASE("has_valid_rebind_error - rebind_error corrupts value type") {
    STATIC_REQUIRE_FALSE(has_valid_rebind_error<ExpectedIntDouble, WrongValueRebindErrorTraits<int, double>>);
}

// ── has_box_traits ────────────────────────────────────────────────────────────

TEST_CASE("has_box_traits - fully correct traits") {
    STATIC_REQUIRE(has_box_traits<ExpectedIntDouble, box_traits<ExpectedIntDouble>>);
}

TEST_CASE("has_box_traits - empty traits passes") {
    STATIC_REQUIRE(has_box_traits<int, box_traits<int>>);
}

TEST_CASE("has_box_traits - bad rebind fails") {
    STATIC_REQUIRE_FALSE(has_box_traits<ExpectedIntDouble, BadRebindTraits<int, double>>);
}

TEST_CASE("has_box_traits - bad rebind_error fails") {
    STATIC_REQUIRE_FALSE(has_box_traits<ExpectedIntDouble, BadRebindErrorTraits<int, double>>);
}

} // namespace beman::monadics::detail::tests
