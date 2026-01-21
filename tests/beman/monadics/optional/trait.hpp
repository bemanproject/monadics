// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_OPTIONAL_TRAIT_HPP
#define BEMAN_MONADICS_OPTIONAL_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <optional>

template <typename T>
struct beman::monadics::box_traits<std::optional<T>> {
    [[nodiscard]] inline static constexpr auto error() noexcept { return std::nullopt; }
};

#endif // BEMAN_MONADICS_OPTIONAL_TRAIT_HPP
