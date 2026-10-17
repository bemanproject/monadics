// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_UNIT_HPP
#define BEMAN_MONADICS_DETAIL_UNIT_HPP

namespace beman::monadics::detail {

struct unit {
    [[nodiscard]] friend constexpr bool operator==(unit, unit) noexcept { return true; }
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_UNIT_HPP
