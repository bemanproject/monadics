# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_library(beman.monadics_headers INTERFACE)
add_library(beman::monadics_headers ALIAS beman.monadics_headers)

target_compile_features(beman.monadics_headers INTERFACE cxx_std_20)

set_target_properties(
    beman.monadics_headers
    PROPERTIES
        VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
)

target_sources(
    beman.monadics_headers
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${PROJECT_SOURCE_DIR}/include"
            FILES
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/access_key.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/and_then.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/as_pointer.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/box_traits.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/decomposable.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_box_traits.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_error_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_error_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_make_error_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_make_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_rebind.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_rebind_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_query_fn.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/get_value_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/instance_of.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/invoke_with_value.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/invoke_with_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_extract_value_type.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_rebind_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/meta_rebind.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/or_else.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/pipe_adaptor.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/propagate_value.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/propagate_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/same_box.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/transform.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/transform_error.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/detail/utility.hpp
                ${PROJECT_SOURCE_DIR}/include/beman/monadics/monadics.hpp
)

add_library(beman.monadics INTERFACE)
add_library(beman::monadics ALIAS beman.monadics)
target_compile_features(beman.monadics INTERFACE cxx_std_20)
target_link_libraries(beman.monadics INTERFACE beman.monadics_headers)
