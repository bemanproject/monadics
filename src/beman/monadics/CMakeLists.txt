# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(GNUInstallDirs)

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
            ../../../include/beman/monadics/detail/decomposable.hpp
            ../../../include/beman/monadics/detail/deduce_error_fn.hpp
            ../../../include/beman/monadics/detail/deduce_error_type.hpp
            ../../../include/beman/monadics/detail/deduce_lift_error_fn.hpp
            ../../../include/beman/monadics/detail/deduce_lift_fn.hpp
            ../../../include/beman/monadics/detail/deduce_rebind.hpp
            ../../../include/beman/monadics/detail/deduce_rebind_error.hpp
            ../../../include/beman/monadics/detail/deduce_value_fn.hpp
            ../../../include/beman/monadics/detail/deduce_value_query_fn.hpp
            ../../../include/beman/monadics/detail/deduce_value_type.hpp
            ../../../include/beman/monadics/detail/extract_value_type.hpp
            ../../../include/beman/monadics/detail/instance_of.hpp
            ../../../include/beman/monadics/detail/meta_extract_value_type.hpp
            ../../../include/beman/monadics/detail/meta_rebind_error.hpp
            ../../../include/beman/monadics/detail/meta_rebind_value.hpp
            ../../../include/beman/monadics/detail/or_else.hpp
            ../../../include/beman/monadics/detail/pipe.hpp
            ../../../include/beman/monadics/detail/same_box.hpp
            ../../../include/beman/monadics/detail/same_template.hpp
            ../../../include/beman/monadics/detail/same_unqualified_as.hpp
            ../../../include/beman/monadics/detail/transform.hpp
            ../../../include/beman/monadics/detail/transform_error.hpp
            ../../../include/beman/monadics/detail/utility.hpp
            ../../../include/beman/monadics/monadics.hpp
            ../../../include/beman/monadics/box_traits.hpp
)

set_target_properties(
    beman.monadics
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME monadics
)

install(
    TARGETS beman.monadics COMPONENT beman.monadics
    EXPORT beman.monadics
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}$<$<CONFIG:Debug>:/debug>
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}$<$<CONFIG:Debug>:/debug>
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(BEMAN_monadics_INSTALL_CONFIG_FILE_PACKAGE)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beman.monadics-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        PATH_VARS PROJECT_NAME PROJECT_VERSION
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/beman.monadics-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        COMPONENT beman.monadics
    )

    install(
        EXPORT beman.monadics
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/beman.monadics"
        NAMESPACE beman::
        FILE beman.monadics-targets.cmake
        COMPONENT beman.monadics
    )
endif()
