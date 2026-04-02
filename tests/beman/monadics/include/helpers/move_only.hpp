// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_HELPERS_MOVE_ONLY_HPP
#define BEMAN_MONADICS_TESTS_HELPERS_MOVE_ONLY_HPP

namespace beman::monadics::helpers {

struct MoveOnly {
    int x = 0;
    explicit MoveOnly(int v) noexcept : x(v) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

} // namespace beman::monadics::helpers

#endif // BEMAN_MONADICS_TESTS_HELPERS_MOVE_ONLY_HPP
