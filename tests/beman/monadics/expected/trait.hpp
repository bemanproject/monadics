// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_EXPECTED_TRAIT_HPP
#define BEMAN_MONADICS_EXPECTED_TRAIT_HPP

#include <beman/monadics/monadics.hpp>

#include <expected>
#include <utility>

namespace beman::monadics {

template<typename T, typename E>
struct box_traits<std::expected<T, E>> {
    static constexpr auto make_error(auto&& e) {
        return std::expected<T, E>{std::unexpected{std::forward<decltype(e)>(e)}};
    }
};

} // namespace beman::monadics

#endif // BEMAN_MONADICS_EXPECTED_TRAIT_HPP
