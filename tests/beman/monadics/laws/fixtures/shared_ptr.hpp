// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_LAWS_FIXTURES_SHARED_PTR_HPP
#define BEMAN_MONADICS_LAWS_FIXTURES_SHARED_PTR_HPP

#include "shared_ptr/trait.hpp"

namespace beman::monadics::tests::shared_box {

using Box = std::shared_ptr<int>;
using Traits = get_box_traits<Box>;

[[nodiscard]] inline bool holds_the_same_value(const Box& a, const Box& b) {
    return static_cast<bool>(a) == static_cast<bool>(b) && (!a || *a == *b);
}

inline const auto f = [](int a) {
    return std::make_shared<int>(a + 1);
};
inline const auto g = [](int a) {
    return std::make_shared<int>(a * 2);
};

[[nodiscard]] inline Box make_engaged() {
    return std::make_shared<int>(10);
}

[[nodiscard]] inline Box make_failed() {
    return Box{};
}

} // namespace beman::monadics::tests::shared_box

#endif // BEMAN_MONADICS_LAWS_FIXTURES_SHARED_PTR_HPP
