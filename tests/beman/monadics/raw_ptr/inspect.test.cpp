// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("runs-on-the-pointee-and-hands-the-pointer-back") {
    int target{10};
    int seen{};
    int* box = &target;

    const auto same = box | inspect([&seen](const int& v) { seen = v; });

    REQUIRE(seen == 10);
    REQUIRE(same == box);
}

TEST_CASE("skipped-when-null") {
    int seen{};
    int* box = nullptr;

    const auto same = box | inspect([&seen](const int& v) { seen = v; });

    REQUIRE(seen == 0);
    REQUIRE(same == nullptr);
}

TEST_CASE("inspect_error-takes-nothing-when-nothing-is-stored") {
    int fired{};
    int* box = nullptr;

    const auto same = box | inspect_error([&fired] { fired = 1; });

    REQUIRE(fired == 1);
    REQUIRE(same == nullptr);
}

TEST_CASE("a-mutable-pointee-is-still-reachable") {
    STATIC_REQUIRE(inspectable<int*&, decltype([](const int&) {})>);
    STATIC_REQUIRE(inspectable<int*&, decltype([](int&) {})>);
}

} // namespace beman::monadics::tests
