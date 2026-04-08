// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

module;

export module beman.monadics;

import beman.monadics.detail;

#define BEMAN_MONADICS_MODULE_INTERFACE

export extern "C++" {
#include "beman/monadics/monadics.hpp"
}
