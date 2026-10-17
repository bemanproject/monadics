// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
#define BEMAN_MONADICS_RAW_PTR_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <type_traits>

template<typename Box>
    requires std::is_pointer_v<Box>
struct beman::monadics::box_traits<Box> {
    using value_type = std::remove_pointer_t<Box>;
    using error_type = unit;

    template<typename V>
    using rebind = V*;

    template<std::same_as<unit>>
    using rebind_error = Box;

    [[nodiscard]] static constexpr bool has_value(const Box box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] static constexpr value_type& value(Box box) noexcept { return *box; }

    // Takes an lvalue reference: T* has no ownership, so make cannot
    // allocate. This means transform (which produces a temporary value)
    // is intentionally disabled for raw pointers.
    [[nodiscard]] static constexpr Box make(value_type& v) noexcept { return &v; }
};

#endif // BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
