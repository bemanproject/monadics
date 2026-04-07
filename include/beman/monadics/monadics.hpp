// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_MONADICS_HPP
#define BEMAN_MONADICS_MONADICS_HPP

#include <beman/monadics/detail/access_key.hpp>
#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/pipe_adaptor.hpp>
#include <beman/monadics/detail/propagate_value.hpp>
#include <beman/monadics/detail/transform.hpp>
#include <beman/monadics/detail/transform_error.hpp>

namespace beman::monadics {

using detail::access_key;
using detail::and_then;
using detail::and_thenable;
using detail::box;
using detail::box_traits;
using detail::get_box_traits;
using detail::has_error_channel;
using detail::invoke_with_error;
using detail::invoke_with_value;
using detail::or_else;
using detail::or_elseable;
using detail::pipe_adaptor;
using detail::propagate_value;
using detail::transform;
using detail::transform_error;
using detail::transform_errorable;
using detail::transformable;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_MONADICS_HPP
