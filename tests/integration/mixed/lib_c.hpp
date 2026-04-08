// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace integration {

// Calls both process_with_headers (lib_a) and process_with_modules (lib_b)
// and returns the sum of their results.
int combined_result();

} // namespace integration
