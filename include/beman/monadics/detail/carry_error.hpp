// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_CARRY_ERROR_HPP
#define BEMAN_MONADICS_DETAIL_CARRY_ERROR_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/make_error.hpp>

#include <utility>

namespace beman::monadics::detail {

struct carry_error_t {
    template<box R>
    [[nodiscard]] constexpr R operator()(auto&& e) const {
        return make_error<R>(std::forward<decltype(e)>(e));
    }
};

inline constexpr carry_error_t carry_error{};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_CARRY_ERROR_HPP
