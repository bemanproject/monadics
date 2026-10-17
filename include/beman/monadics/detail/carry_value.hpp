// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_CARRY_VALUE_HPP
#define BEMAN_MONADICS_DETAIL_CARRY_VALUE_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/make_value.hpp>

#include <utility>

namespace beman::monadics::detail {

struct carry_value_t {
    template<box R>
    [[nodiscard]] constexpr R operator()(auto&&... v) const {
        return make_value<R>(std::forward<decltype(v)>(v)...);
    }
};

inline constexpr carry_value_t carry_value{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_CARRY_VALUE_HPP
