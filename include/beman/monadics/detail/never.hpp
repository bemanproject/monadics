// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_NEVER_HPP
#define BEMAN_MONADICS_DETAIL_NEVER_HPP

namespace beman::monadics::detail {

struct never {
    never() = delete;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_NEVER_HPP
