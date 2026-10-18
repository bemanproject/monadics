// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_RESULT_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_RESULT_HPP

#include "result/trait.hpp"

namespace beman::monadics::tests::result_box {

using Box = Result<double, int>;
using Traits = get_box_traits<Box>;

constexpr auto f = [](double a) {
    return Box{a + 1.0};
};
constexpr auto g = [](double a) {
    return Box{a * 2.0};
};

constexpr Box engaged{10.0};
constexpr Box failed{7};

} // namespace beman::monadics::tests::result_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_RESULT_HPP
