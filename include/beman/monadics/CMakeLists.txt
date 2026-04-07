# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

target_sources(
    beman.monadics
    PUBLIC
        FILE_SET HEADERS
            FILES
                detail/access_key.hpp
                detail/and_then.hpp
                detail/as_pointer.hpp
                detail/box_traits.hpp
                detail/decomposable.hpp
                detail/get_box_traits.hpp
                detail/get_error_fn.hpp
                detail/get_error_type.hpp
                detail/get_make_error_fn.hpp
                detail/get_make_fn.hpp
                detail/get_rebind.hpp
                detail/get_rebind_error.hpp
                detail/get_value_fn.hpp
                detail/get_value_query_fn.hpp
                detail/get_value_type.hpp
                detail/instance_of.hpp
                detail/invoke_with_value.hpp
                detail/invoke_with_error.hpp
                detail/meta_extract_value_type.hpp
                detail/meta_rebind_error.hpp
                detail/meta_rebind.hpp
                detail/or_else.hpp
                detail/pipe_adaptor.hpp
                detail/propagate_value.hpp
                detail/propagate_error.hpp
                detail/same_box.hpp
                detail/transform.hpp
                detail/transform_error.hpp
                detail/utility.hpp
                monadics.hpp
)
