// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_ACCESS_KEY_HPP
#define BEMAN_MONADICS_DETAIL_ACCESS_KEY_HPP

namespace beman::monadics::detail {

template<class T>
class access_key {
    friend T;
    access_key() = default;
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_ACCESS_KEY_HPP
