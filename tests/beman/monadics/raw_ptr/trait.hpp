// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
#define BEMAN_MONADICS_RAW_PTR_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <concepts>

template<typename T>
struct beman::monadics::box_traits<T*> {
    using value_type = T;
    using error_type = unit;

    template<typename V>
    using rebind = V*;

    template<std::same_as<unit>>
    using rebind_error = T*;

    [[nodiscard]] static constexpr bool has_value(T* const box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] static constexpr value_type& value(T* box) noexcept { return *box; }

    // Takes an lvalue reference: T* has no ownership, so make cannot
    // allocate. This means transform (which produces a temporary value)
    // is intentionally disabled for raw pointers.
    [[nodiscard]] static constexpr T* make(value_type& v) noexcept { return &v; }
};

#endif // BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
