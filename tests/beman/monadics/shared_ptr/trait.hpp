// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_OPTIONAL_TRAIT_HPP
#define BEMAN_MONADICS_OPTIONAL_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <memory>

template <typename T>
struct beman::monadics::box_traits<std::shared_ptr<T>> {
    [[nodiscard]] inline static constexpr bool has_value(const std::shared_ptr<T>& box) noexcept {
        return static_cast<bool>(box);
    }

    [[nodiscard]] inline static constexpr decltype(auto) value(auto&& box) noexcept {
        return *std::forward<decltype(box)>(box);
    }

    [[nodiscard]] inline static constexpr auto error() noexcept { return nullptr; }

    [[nodiscard]] inline static constexpr decltype(auto) make(auto&& value) noexcept {
        return std::make_shared<T>(std::forward<decltype(value)>(value));
    }
};

#endif // BEMAN_MONADICS_OPTIONAL_TRAIT_HPP
