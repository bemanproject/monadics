// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_RAW_PTR_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_RAW_PTR_HPP

#include "raw_ptr/trait.hpp"

namespace beman::monadics::tests::raw_box {

using Box = int*;
using Traits = get_box_traits<Box>;

} // namespace beman::monadics::tests::raw_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_RAW_PTR_HPP
