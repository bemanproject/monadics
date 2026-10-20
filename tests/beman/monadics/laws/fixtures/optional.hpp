// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_OPTIONAL_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_OPTIONAL_HPP

#include "optional/trait.hpp"

namespace beman::monadics::tests::opt_box {

using Box = std::optional<int>;
using Traits = get_box_traits<Box>;

constexpr auto f = [](int a) {
    return Box{a + 1};
};
constexpr auto g = [](int a) {
    return Box{a * 2};
};

constexpr Box engaged{10};
constexpr Box failed{};

} // namespace beman::monadics::tests::opt_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_OPTIONAL_HPP
