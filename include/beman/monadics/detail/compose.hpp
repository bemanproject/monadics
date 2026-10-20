// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_DETAIL_COMPOSE_HPP
#define BEMAN_MONADICS_DETAIL_COMPOSE_HPP

#include <concepts>
#include <type_traits>
#include <utility>

namespace beman::monadics::detail {

namespace _compose {

template<typename G, typename F, typename... Args>
concept composable = std::invocable<F, Args...>
                  && ((std::is_void_v<std::invoke_result_t<F, Args...>> && std::invocable<G>)
                      || std::invocable<G, std::invoke_result_t<F, Args...>>);

template<typename G, typename F, typename... Args>
    requires composable<G, F, Args...>
[[nodiscard]] constexpr auto call(G&& g, F&& f, Args&&... args) {
    if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
        std::forward<F>(f)(std::forward<Args>(args)...);
        return std::forward<G>(g)();
    } else {
        return std::forward<G>(g)(std::forward<F>(f)(std::forward<Args>(args)...));
    }
}

} // namespace _compose

template<typename G, typename F>
class composed {
  public:
    constexpr composed(G g, F f) : g_(std::move(g)), f_(std::move(f)) {}

    template<typename... Args>
        requires _compose::composable<const G&, const F&, Args...>
    [[nodiscard]] constexpr auto operator()(Args&&... args) const& {
        return _compose::call(g_, f_, std::forward<Args>(args)...);
    }

    template<typename... Args>
        requires _compose::composable<G, F, Args...>
    [[nodiscard]] constexpr auto operator()(Args&&... args) && {
        return _compose::call(std::move(g_), std::move(f_), std::forward<Args>(args)...);
    }

  private:
    G g_;
    F f_;
};

template<typename G, typename F>
[[nodiscard]] constexpr composed<G, F> compose(G g, F f) {
    return composed<G, F>{std::move(g), std::move(f)};
}

} // namespace beman::monadics::detail

#endif // BEMAN_MONADICS_DETAIL_COMPOSE_HPP
