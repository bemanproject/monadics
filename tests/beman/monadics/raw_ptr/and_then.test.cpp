// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

namespace {

struct Foo {
    int value{};
};

struct Boo {
    Foo* foo{nullptr};
};

} // namespace

TEST_CASE("with-value") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([&](auto&& b) { return b.foo; }) | and_then([](Foo& f) {
            f.value += 5;
            return &f;
        });

        return result->value;
    }();

    STATIC_REQUIRE(value == 10);
}

TEST_CASE("with-null") {
    constexpr int value = []() {
        int* p{nullptr};

        auto result = p | and_then([&](auto&& v) { return &v; });

        return result ? 10 : -10;
    }();

    STATIC_REQUIRE(value == -10);
}

TEST_CASE("self") {
    constexpr int value = []() {
        Foo foo{.value = 5};
        Boo boo{.foo = &foo};

        auto result = &boo | and_then([](auto&& b) { return &b; }) | and_then([](auto&& b) { return &b; });
        return result->foo->value;
    }();

    STATIC_REQUIRE(value == 5);
}

// raw_ptr::value() takes the pointer by value and returns *ptr as T&, so the
// box's own value category is irrelevant: moving a T* just copies the pointer word.
TEMPLATE_TEST_CASE_SIG("value-is-always-lvalue-ref",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           int*&,
                           [](int&) { return (int*)nullptr; },
                           true
                       ),
                       (
                           int*&&,
                           [](int&) { return (int*)nullptr; },
                           true
                       ),
                       (
                           int*&,
                           [](int&&) { return (int*)nullptr; },
                           false
                       ),
                       (int*&&, [](int&&) { return (int*)nullptr; }, false)) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

TEST_CASE("pointee-is-mutable-through-lvalue-ref") {
    constexpr auto result = [] {
        int val = 10;
        int* r = &val | and_then([](int& v) {
            v *= 2;
            return &v;
        });
        return std::pair{val, *r};
    }();

    STATIC_REQUIRE(result.first == 20);
    STATIC_REQUIRE(result.second == 20);
}

} // namespace beman::monadics::tests
