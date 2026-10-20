// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_BINDS_TEMPORARY_HPP
#define BEMAN_MONADICS_DETAIL_BINDS_TEMPORARY_HPP

#include <type_traits>

namespace beman::monadics::detail {

#ifdef __cpp_lib_reference_from_temporary

template<typename To, typename From>
concept binds_temporary = std::reference_constructs_from_temporary_v<To, From>;

#else

template<typename To, typename From>
concept binds_temporary =
    std::is_reference_v<To>
    && ((!std::is_reference_v<From> && std::is_convertible_v<std::remove_cvref_t<From>*, std::remove_cvref_t<To>*>)
        || (std::is_lvalue_reference_v<To>
            && std::is_const_v<std::remove_reference_t<To> >
            && std::is_convertible_v<From, const std::remove_cvref_t<To>&&>
            && !std::is_convertible_v<From, std::remove_cvref_t<To>&>));

#endif

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_BINDS_TEMPORARY_HPP
