// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_EXPECTED_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_EXPECTED_HPP

#include "expected/trait.hpp"

namespace beman::monadics::tests::expected_box {

using Box = std::expected<int, int>;
using Traits = get_box_traits<Box>;

constexpr auto f = [](int a) {
    return Box{a + 1};
};
constexpr auto g = [](int a) {
    return Box{a * 2};
};

constexpr Box engaged{10};
constexpr Box failed{std::unexpect, 7};

} // namespace beman::monadics::tests::expected_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_EXPECTED_HPP
