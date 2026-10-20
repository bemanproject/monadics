// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_UNREACHABLE_HPP
#define BEMAN_MONADICS_DETAIL_UNREACHABLE_HPP

#include <utility>

namespace beman::monadics::detail {

[[noreturn]] inline void unreachable() {
#ifdef __cpp_lib_unreachable
    std::unreachable();
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#endif
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_UNREACHABLE_HPP
