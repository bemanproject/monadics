// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
#define BEMAN_MONADICS_RAW_PTR_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <type_traits>

template<typename Box>
    requires std::is_pointer_v<Box>
struct beman::monadics::box_traits<Box> {
    using value_type = std::remove_pointer_t<Box>;

    template<typename V>
    using rebind = V*;

    template<typename>
    using rebind_error = Box;

    [[nodiscard]] static constexpr bool has_value(const Box box) noexcept { return static_cast<bool>(box); }

    [[nodiscard]] static constexpr value_type& value(Box box) noexcept { return *box; }

    [[nodiscard]] static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] static constexpr Box make(value_type& v) noexcept { return &v; }

    [[nodiscard]] static constexpr Box make_error() noexcept { return nullptr; }
};

#endif // BEMAN_MONADICS_RAW_PTR_TRAIT_HPP
