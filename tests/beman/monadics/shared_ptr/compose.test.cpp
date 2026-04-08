// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <catch2/catch_test_macros.hpp>

#include "trait.hpp"

namespace beman::monadics::tests {

TEST_CASE("with-null-then-else-transform") {
    const auto result = std::shared_ptr<int>{}
                      | and_then([](auto&& v) { return std::make_shared<int>(v + 10); })
                      | or_else([]() { return std::make_shared<int>(-10); })
                      | transform([](auto&& v) { return v + 50; });
    REQUIRE(*result == 40);
}

TEST_CASE("with-value-then-else-transform") {
    const auto result = std::make_shared<int>(1)
                      | and_then([](auto&& v) { return std::make_shared<int>(v + 10); })
                      | or_else([]() { return std::make_shared<int>(-10); })
                      | transform([](auto&& v) { return v + 50; });
    REQUIRE(*result == 61);
}

} // namespace beman::monadics::tests
