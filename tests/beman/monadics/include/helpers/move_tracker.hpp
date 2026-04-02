// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_TESTS_HELPERS_MOVE_TRACKER_HPP
#define BEMAN_MONADICS_TESTS_HELPERS_MOVE_TRACKER_HPP

namespace beman::monadics::helpers {

struct MoveTracker {
    int copies{};
    int moves{};

    constexpr MoveTracker() = default;

    constexpr MoveTracker(const MoveTracker& o) noexcept : copies(o.copies + 1), moves(o.moves) {}

    constexpr MoveTracker(MoveTracker&& o) noexcept : copies(o.copies), moves(o.moves + 1) {}
};

} // namespace beman::monadics::helpers

#endif // BEMAN_MONADICS_TESTS_HELPERS_MOVE_TRACKER_HPP
