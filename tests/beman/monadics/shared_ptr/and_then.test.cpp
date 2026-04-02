// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include "beman/monadics/detail/and_then.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("with-value") {
    const auto result =
        std::make_shared<int>(10) | and_then([](auto&& value) { return std::make_shared<double>(value * 2.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result));
    REQUIRE(*result == 20.0);
}

TEST_CASE("with-nullptr") {
    const auto result =
        std::shared_ptr<int>{} | and_then([](auto&& value) { return std::make_shared<double>(value * 1.0); });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<double>>);
    REQUIRE(static_cast<bool>(result) == false);
}

struct Foo {};

struct Boo {
    std::shared_ptr<Foo> foo;
};

TEST_CASE("with-value-return-member") {
    const auto result =
        std::make_shared<Boo>() | and_then([](auto&& boo) { return std::forward<decltype(boo)>(boo).foo; });
    STATIC_REQUIRE(std::same_as<decltype(result), const std::shared_ptr<Foo>>);
    REQUIRE(static_cast<bool>(result) == false);
}

TEMPLATE_TEST_CASE_SIG("value-is-always-lvalue-ref",
                       "",
                       ((typename Box, auto Fn, bool Expected), Box, Fn, Expected),
                       (
                           std::shared_ptr<int>&,
                           [](int&) { return std::shared_ptr<int>{}; },
                           true),
                       (
                           std::shared_ptr<int>&&,
                           [](int&) { return std::shared_ptr<int>{}; },
                           true),
                       (
                           std::shared_ptr<int>&,
                           [](int&&) { return std::shared_ptr<int>{}; },
                           false),
                       (std::shared_ptr<int>&&, [](int&&) { return std::shared_ptr<int>{}; }, false)) {
    STATIC_REQUIRE(and_thenable<Box, decltype(Fn)> == Expected);
}

TEST_CASE("pointee-is-mutable-through-lvalue-ref") {
    auto sp = std::make_shared<int>(5);
    auto result = sp | and_then([](int& v) {
                      v *= 3;
                      return std::make_shared<int>(v);
                  });
    REQUIRE(*sp == 15);
    REQUIRE(*result == 15);
}

} // namespace beman::monadics::tests
