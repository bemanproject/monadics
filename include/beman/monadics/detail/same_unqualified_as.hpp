#pragma once

#include <concepts>
#include <type_traits>

namespace beman::monadics::detail {

template<typename T, typename U>
concept same_unqualified_as = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

} // namespace beman::monadics::detail
