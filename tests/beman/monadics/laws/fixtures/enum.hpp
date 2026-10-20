// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_ENUM_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_ENUM_HPP

#include "enum/trait.hpp"

namespace beman::monadics::tests::enum_box {

using Box = CURLcode;
using Traits = get_box_traits<Box>;

constexpr auto f = [] {
    return Box{CURLE_OK};
};
constexpr auto g = [] {
    return Box{CURLE_OK};
};

constexpr Box engaged{CURLE_OK};
constexpr Box failed{CURLE_NOT_BUILT_IN};

} // namespace beman::monadics::tests::enum_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_ENUM_HPP
