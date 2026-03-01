# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_library(beman.monadics INTERFACE)
add_library(beman::monadics ALIAS beman.monadics)

target_sources(
    beman.monadics
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ../../../include
        FILES
            ../../../include/beman/monadics/detail/and_then.hpp
            ../../../include/beman/monadics/detail/as_pointer.hpp
            ../../../include/beman/monadics/detail/box_traits.hpp
            ../../../include/beman/monadics/detail/decomposable.hpp
            ../../../include/beman/monadics/detail/get_box_traits.hpp
            ../../../include/beman/monadics/detail/get_error_fn.hpp
            ../../../include/beman/monadics/detail/get_error_type.hpp
            ../../../include/beman/monadics/detail/get_make_error_fn.hpp
            ../../../include/beman/monadics/detail/get_make_fn.hpp
            ../../../include/beman/monadics/detail/get_rebind.hpp
            ../../../include/beman/monadics/detail/get_rebind_error.hpp
            ../../../include/beman/monadics/detail/get_value_fn.hpp
            ../../../include/beman/monadics/detail/get_value_query_fn.hpp
            ../../../include/beman/monadics/detail/get_value_type.hpp
            ../../../include/beman/monadics/detail/extract_value_type.hpp
            ../../../include/beman/monadics/detail/instance_of.hpp
            ../../../include/beman/monadics/detail/invoke_with_value.hpp
            ../../../include/beman/monadics/detail/invoke_with_error.hpp
            ../../../include/beman/monadics/detail/meta_extract_value_type.hpp
            ../../../include/beman/monadics/detail/meta_rebind_error.hpp
            ../../../include/beman/monadics/detail/meta_rebind_value.hpp
            ../../../include/beman/monadics/detail/or_else.hpp
            ../../../include/beman/monadics/detail/rebox_value.hpp
            ../../../include/beman/monadics/detail/rebox_error.hpp
            ../../../include/beman/monadics/detail/same_box.hpp
            ../../../include/beman/monadics/detail/same_template.hpp
            ../../../include/beman/monadics/detail/same_unqualified_as.hpp
            ../../../include/beman/monadics/detail/transform.hpp
            ../../../include/beman/monadics/detail/transform_error.hpp
            ../../../include/beman/monadics/detail/utility.hpp
            ../../../include/beman/monadics/monadics.hpp
)

set_target_properties(
    beman.monadics
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME monadics
)
