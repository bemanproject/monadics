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

if(BEMAN_USE_MODULES)
    add_library(beman.monadics_detail)
    target_compile_features(
        beman.monadics_detail
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )
    target_sources(
        beman.monadics_detail
        PUBLIC
            FILE_SET CXX_MODULES FILES ${CMAKE_CURRENT_SOURCE_DIR}/detail.cppm
    )
    target_link_libraries(beman.monadics_detail PUBLIC beman.monadics_headers)
    set_target_properties(
        beman.monadics_detail
        PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VERIFY_INTERFACE_HEADER_SETS OFF
            VISIBILITY_INLINES_HIDDEN ON
    )

    add_library(beman.monadics_module)
    target_compile_features(
        beman.monadics_module
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )
    target_sources(
        beman.monadics_module
        PUBLIC
            FILE_SET CXX_MODULES FILES ${CMAKE_CURRENT_SOURCE_DIR}/monadics.cppm
    )
    target_link_libraries(beman.monadics_module PRIVATE beman.monadics_detail)
    target_link_libraries(beman.monadics_module PUBLIC beman.monadics_headers)
    add_library(beman::monadics_module ALIAS beman.monadics_module)

    # --------------------------------------------------------------------------
    # beman::monadics — smart default target in module mode.
    #
    # This is a real library (not INTERFACE) that compiles monadics.cppm
    # itself. This is necessary for correct behaviour after find_package:
    # CMake only generates the module BMI compile rule for non-INTERFACE imported
    # targets. An INTERFACE wrapper that merely links the module target transitively
    # does not work because CMake does not walk INTERFACE_LINK_LIBRARIES on
    # imported INTERFACE targets when collecting IMPORTED_CXX_MODULES_<CONFIG>.
    #
    # Both beman.monadics and beman.monadics_module compile monadics.cppm.
    # This double compilation is intentional: it makes the two targets
    # independent. No consumer links both simultaneously, so there is no
    # duplicate-module-provider conflict.
    #
    # BEMAN_USE_MODULES propagates to consumers so that
    # #include <beman/monadics/monadics.hpp> transparently becomes
    # `import beman.monadics`. This is intentional and project-wide.
    add_library(beman.monadics)
    add_library(beman::monadics ALIAS beman.monadics)
    target_compile_features(beman.monadics PUBLIC cxx_std_${CMAKE_CXX_STANDARD})
    target_sources(
        beman.monadics
        PUBLIC
            FILE_SET CXX_MODULES FILES ${CMAKE_CURRENT_SOURCE_DIR}/monadics.cppm
    )
    target_link_libraries(beman.monadics PRIVATE beman.monadics_detail)
    target_link_libraries(beman.monadics PUBLIC beman.monadics_headers)
    target_compile_definitions(beman.monadics INTERFACE BEMAN_USE_MODULES)
    set_target_properties(
        beman.monadics
        PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VERIFY_INTERFACE_HEADER_SETS OFF
            VISIBILITY_INLINES_HIDDEN ON
    )

    if(MSVC)
        set_source_files_properties(
            ${CMAKE_CURRENT_SOURCE_DIR}/detail.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/monadics.cppm
            PROPERTIES COMPILE_OPTIONS "/wd5244"
        )
    else()
        set_source_files_properties(
            ${CMAKE_CURRENT_SOURCE_DIR}/detail.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/monadics.cppm
            PROPERTIES COMPILE_OPTIONS "-Wno-include-angled-in-module-purview"
        )
    endif()
else()
    add_library(beman.monadics INTERFACE)
    add_library(beman::monadics ALIAS beman.monadics)
    target_compile_features(beman.monadics INTERFACE cxx_std_20)
    target_link_libraries(beman.monadics INTERFACE beman.monadics_headers)
endif()
