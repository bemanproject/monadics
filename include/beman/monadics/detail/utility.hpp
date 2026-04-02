// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_UTILITY_HPP
#define BEMAN_MONADICS_DETAIL_UTILITY_HPP

#include <cstddef>
#include <type_traits>

namespace beman::monadics::detail {

template<typename T>
concept False = false;

template<std::size_t N>
struct msg {
    char data[N];

    explicit(false) consteval msg(const char (&m)[N]) noexcept {
        for (std::size_t i{}; i < N; i++) {
            data[i] = m[i];
        }
    }
};

template<std::size_t N>
msg(const char (&str)[N]) -> msg<N>;

template<msg>
concept on_error = false;

template<typename T>
concept deduced = !std::is_void_v<T>;

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_UTILITY_HPP
