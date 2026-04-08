// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lib_c.hpp"

#undef BEMAN_USE_MODULES

#include <beman/monadics/monadics.hpp>

import beman.monadics;

int main() {
    return integration::combined_result() == 25 ? 0 : 1;
}
