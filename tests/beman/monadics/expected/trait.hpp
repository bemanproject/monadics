// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_EXPECTED_TRAIT_HPP
#define BEMAN_MONADICS_EXPECTED_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <concepts>
#include <expected>
#include <type_traits>
#include <utility>

namespace beman::monadics {

template<typename T, typename E>
struct box_traits<std::expected<T, E>> {
    template<typename G>
        requires std::constructible_from<std::expected<T, E>, std::unexpected<std::remove_cvref_t<G>>>
    static constexpr auto make_error(G&& e) {
        return std::expected<T, E>{std::unexpected{std::forward<G>(e)}};
    }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_EXPECTED_TRAIT_HPP
