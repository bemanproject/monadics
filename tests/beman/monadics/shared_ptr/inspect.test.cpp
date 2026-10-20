// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "trait.hpp"

#include <catch2/catch_test_macros.hpp>

namespace beman::monadics::tests {

TEST_CASE("runs-on-the-pointee-and-hands-the-pointer-back") {
    int seen{};
    const auto box = std::make_shared<int>(10);

    const auto same = box | inspect([&seen](const int& v) { seen = v; });

    REQUIRE(seen == 10);
    REQUIRE(same == box);
}

TEST_CASE("skipped-when-null") {
    int seen{};
    const std::shared_ptr<int> box{};

    const auto same = box | inspect([&seen](const int& v) { seen = v; });

    REQUIRE(seen == 0);
    REQUIRE(same == nullptr);
}

TEST_CASE("inspect_error-takes-nothing-when-nothing-is-stored") {
    int fired{};
    const std::shared_ptr<int> box{};

    const auto same = box | inspect_error([&fired] { fired = 1; });

    REQUIRE(fired == 1);
    REQUIRE(same == nullptr);
}

} // namespace beman::monadics::tests
