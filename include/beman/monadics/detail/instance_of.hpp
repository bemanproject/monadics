// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_INSTANCE_OF_HPP
#define BEMAN_MONADICS_DETAIL_INSTANCE_OF_HPP

#include <beman/monadics/detail/as_pointer.hpp>
#include <beman/monadics/detail/decomposable.hpp>

namespace beman::monadics::detail {

template<typename T, template<typename...> class U>
concept instance_of = requires {
    requires decomposable<T, 1>;
    []<typename... Ts>(U<Ts...>*) {
    }(as_pointer<T>);
};

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_INSTANCE_OF_HPP
