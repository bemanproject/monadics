// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_MONADICS_MONADICS_HPP
#define BEMAN_MONADICS_MONADICS_HPP

#include <beman/monadics/detail/and_then.hpp>
#include <beman/monadics/detail/callable_adaptor.hpp>
#include <beman/monadics/detail/carry_error.hpp>
#include <beman/monadics/detail/carry_value.hpp>
#include <beman/monadics/detail/compose.hpp>
#include <beman/monadics/detail/either.hpp>
#include <beman/monadics/detail/get_box_traits.hpp>
#include <beman/monadics/detail/inspect.hpp>
#include <beman/monadics/detail/inspect_error.hpp>
#include <beman/monadics/detail/invoke_with_error.hpp>
#include <beman/monadics/detail/invoke_with_value.hpp>
#include <beman/monadics/detail/just.hpp>
#include <beman/monadics/detail/just_error.hpp>
#include <beman/monadics/detail/make_error.hpp>
#include <beman/monadics/detail/make_value.hpp>
#include <beman/monadics/detail/never.hpp>
#include <beman/monadics/detail/or_else.hpp>
#include <beman/monadics/detail/to.hpp>
#include <beman/monadics/detail/transform.hpp>
#include <beman/monadics/detail/transform_error.hpp>
#include <beman/monadics/detail/unit.hpp>
#include <beman/monadics/detail/unreachable.hpp>

namespace beman::monadics {

using detail::and_then;
using detail::and_thenable;
using detail::box;
using detail::box_traits;
using detail::callable_adaptor;
using detail::carry_error;
using detail::carry_value;
using detail::compose;
using detail::either;
using detail::either_result_t;
using detail::eitherable;
using detail::error_only;
using detail::get_box_traits;
using detail::has_error_channel;
using detail::inspect;
using detail::inspect_error;
using detail::inspect_errorable;
using detail::inspectable;
using detail::invoke_with_error;
using detail::invoke_with_value;
using detail::just;
using detail::just_error;
using detail::just_error_t;
using detail::just_t;
using detail::make_error;
using detail::make_value;
using detail::never;
using detail::no_case;
using detail::no_error_case;
using detail::no_value_case;
using detail::or_else;
using detail::or_elseable;
using detail::transform;
using detail::transform_error;
using detail::transform_errorable;
using detail::transformable;
using detail::unit;
using detail::unreachable;
using detail::value_only;

} // namespace beman::monadics

#endif // BEMAN_MONADICS_MONADICS_HPP
