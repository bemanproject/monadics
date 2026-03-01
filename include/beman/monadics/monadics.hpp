// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_MONADICS_HPP
#define BEMAN_MONADICS_MONADICS_HPP

#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/transform.hpp>
#include <beman/monadics/detail/transform_error.hpp>

namespace beman::monadics {

using detail::and_then;
using detail::box_traits;
using detail::get_box_traits;
using detail::or_else;
using detail::transform;
using detail::transform_error;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_MONADICS_HPP
