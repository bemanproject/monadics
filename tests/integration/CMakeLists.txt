# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(_integration_common_options
    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
    "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

set(_integration_strict_options
    "-DCMAKE_CXX_EXTENSIONS=OFF"
    "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
)

function(beman_add_fetchcontent_test name)
    add_test(
        NAME "beman.monadics.integration.fetchcontent.${name}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/fetchcontent"
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent/${name}" --build-generator
            "${CMAKE_GENERATOR}" --build-options
            "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            ${_integration_common_options} ${ARGN} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent/${name}/beman.monadics.integration.fetchcontent"
    )
endfunction()

function(beman_add_find_package_install mode use_modules)
    add_test(
        NAME "beman.monadics.integration.find_package.install.${mode}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test "${PROJECT_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/find_package/provider/${mode}"
            --build-generator "${CMAKE_GENERATOR}" --build-options
            "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/find_package/prefix/${mode}"
            "-DBEMAN_USE_MODULES=${use_modules}"
            "-DBEMAN_MONADICS_BUILD_TESTS=OFF"
            "-DBEMAN_MONADICS_BUILD_EXAMPLES=OFF" ${_integration_common_options}
            --test-command "${CMAKE_COMMAND}" --install
            "${CMAKE_CURRENT_BINARY_DIR}/find_package/provider/${mode}"
    )
    set_tests_properties(
        "beman.monadics.integration.find_package.install.${mode}"
        PROPERTIES
            FIXTURES_SETUP
                "beman.monadics.integration.find_package.fixture.${mode}"
    )
endfunction()

function(beman_add_find_package_test name mode)
    add_test(
        NAME "beman.monadics.integration.find_package.${name}"
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/find_package"
            "${CMAKE_CURRENT_BINARY_DIR}/find_package/${name}" --build-generator
            "${CMAKE_GENERATOR}" --build-options
            "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/find_package/prefix/${mode}"
            ${_integration_common_options} ${ARGN} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/find_package/${name}/beman.monadics.integration.find_package"
    )
    set_tests_properties(
        "beman.monadics.integration.find_package.${name}"
        PROPERTIES
            FIXTURES_REQUIRED
                "beman.monadics.integration.find_package.fixture.${mode}"
    )
endfunction()

# FetchContent tests
beman_add_fetchcontent_test(
    headers
    "-DBEMAN_USE_MODULES=OFF"
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_headers"
)
beman_add_fetchcontent_test(
    headers.strict
    "-DBEMAN_USE_MODULES=OFF"
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_headers"
    ${_integration_strict_options}
)
beman_add_fetchcontent_test(
    compatible
    "-DBEMAN_USE_MODULES=OFF"
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
)
beman_add_fetchcontent_test(
    compatible.strict
    "-DBEMAN_USE_MODULES=OFF"
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
    ${_integration_strict_options}
)
if(BEMAN_USE_MODULES)
    beman_add_fetchcontent_test(
        modules
        "-DBEMAN_USE_MODULES=ON"
        "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_module"
        "-DBEMAN_INTEGRATION_USE_IMPORT_SOURCE=ON"
    )
    beman_add_fetchcontent_test(
        compatible.modules
        "-DBEMAN_USE_MODULES=ON"
        "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
    )
endif()

# find_package install fixtures (self-contained builds)
beman_add_find_package_install(headers OFF)

# find_package consumer tests (headers install)
beman_add_find_package_test(
    headers
    headers
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_headers"
)
beman_add_find_package_test(
    headers.strict
    headers
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_headers"
    ${_integration_strict_options}
)
beman_add_find_package_test(
    compatible
    headers
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
)
beman_add_find_package_test(
    compatible.strict
    headers
    "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
    ${_integration_strict_options}
)

if(BEMAN_USE_MODULES)
    # Modules install fixture + its consumers
    beman_add_find_package_install(modules ON)

    beman_add_find_package_test(
        modules
        modules
        "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics_module"
        "-DBEMAN_INTEGRATION_USE_IMPORT_SOURCE=ON"
        "-DBEMAN_USE_MODULES=ON"
    )
    beman_add_find_package_test(
        compatible.modules
        modules
        "-DBEMAN_INTEGRATION_CONSUMER_TARGET=beman::monadics"
        "-DBEMAN_USE_MODULES=ON"
    )

    # Mixed: lib A (headers) + lib B (modules) → lib C → executable D.
    # BEMAN_USE_MODULES is forced ON inside the inner project.
    add_test(
        NAME beman.monadics.integration.mixed
        COMMAND
            ${CMAKE_CTEST_COMMAND} --build-and-test
            "${CMAKE_CURRENT_SOURCE_DIR}/mixed"
            "${CMAKE_CURRENT_BINARY_DIR}/mixed" --build-generator
            "${CMAKE_GENERATOR}" --build-options
            "-DBEMAN_MONADICS_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            ${_integration_common_options} --test-command
            "${CMAKE_CURRENT_BINARY_DIR}/mixed/beman.monadics.integration.mixed"
    )
endif()
