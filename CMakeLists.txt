# Copyright 2011-2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

# Select the release build type by default to get optimization flags.
# This has to come before project() which otherwise initializes it.
# Build type can still be overridden by setting -DCMAKE_BUILD_TYPE=
set(CMAKE_BUILD_TYPE
    "Release"
    CACHE STRING "")

########################################################################
# Project setup
########################################################################
# Make sure this version matches ${GR_CMAKE_MIN_VERSION} (a variable can't be
# used here).
cmake_minimum_required(VERSION 3.16)
project(gnuradio CXX C)
enable_testing()
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake/Modules)


# Incorporate CMake function/macros overloading.
include(CMakeOverloads)

# Add more build types and check that requested build type is valid
include(GrBuildTypes)
gr_check_build_type(${CMAKE_BUILD_TYPE})
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}.")

# Set the build date
string(TIMESTAMP BUILD_DATE "%a, %d %b %Y %H:%M:%SZ" UTC)
string(TIMESTAMP BUILD_DATE_SHORT "%Y-%m-%d" UTC)
message(STATUS "Build date set to ${BUILD_DATE}.")

include(GrComponent)

# Set the version information here
# cmake-format: off
SET(VERSION_MAJOR 3)
SET(VERSION_API   11)
SET(VERSION_ABI   0)
SET(VERSION_PATCH git)
include(GrVersion) # setup version info
# cmake-format: on

# Set minimum version requirements
include(GrMinReq)
########################################################################
# Setup Boost for global use (within this build)
# Do this before enabling testing support, as it depends
# on unit_test_framework
########################################################################
include(GrBoost)
gr_register_component(
    NAME "testing-support"
    VAR ENABLE_TESTING
    REQUIRED_TPLS Boost_UNIT_TEST_FRAMEWORK_FOUND
)

# Enable generation of compile_commands.json for code completion engines
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set wiki block docs url prefix used in grc-docs.conf (block name gets appended to end of this string)
set(GRC_DOCS_URL_PREFIX "https://wiki.gnuradio.org/index.php/")

########################################################################
# Environment setup
########################################################################
if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT "${CMAKE_INSTALL_PREFIX}")
endif()

########################################################################
# Import executables from a native build (for cross compiling)
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
if(IMPORT_EXECUTABLES)
    include(${IMPORT_EXECUTABLES})
endif(IMPORT_EXECUTABLES)

# set file that the native build will fill with exports
set(EXPORT_FILE ${PROJECT_BINARY_DIR}/ImportExecutables.cmake)
file(WRITE ${EXPORT_FILE} ) # blank the file (subdirs will append)

########################################################################
# Compiler settings
########################################################################

include(GrCompilerSettings)

# Record Compiler Info for record
string(TOUPPER ${CMAKE_BUILD_TYPE} GRCBTU)
set(COMPILER_INFO "")
if(MSVC)
    if(MSVC_VERSION LESS "${MSVC_MIN_VERSION}")
        message(FATAL_ERROR "MSVC Versions < minimum version ${MSVC_MIN_VERSION}")
    endif()
    set(cmake_c_compiler_version ${MSVC_VERSION})
    set(cmake_cxx_compiler_version ${MSVC_VERSION})
else()
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version
                    OUTPUT_VARIABLE cmake_c_compiler_version)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
                    OUTPUT_VARIABLE cmake_cxx_compiler_version)
endif(MSVC)
set(COMPILER_INFO
    "${CMAKE_C_COMPILER}:::${CMAKE_C_FLAGS_${GRCBTU}} ${CMAKE_C_FLAGS}\n${CMAKE_CXX_COMPILER}:::${CMAKE_CXX_FLAGS_${GRCBTU}} ${CMAKE_CXX_FLAGS}\n"
)

# Convert to a C string to compile and display properly
string(STRIP "${cmake_c_compiler_version}" cmake_c_compiler_version)
string(STRIP "${cmake_cxx_compiler_version}" cmake_cxx_compiler_version)
string(STRIP ${COMPILER_INFO} COMPILER_INFO)
message(STATUS "Compiler Version: ${cmake_c_compiler_version}")
message(STATUS "Compiler Flags: ${COMPILER_INFO}")
string(REPLACE "\n" " \\n" cmake_c_compiler_version ${cmake_c_compiler_version})
string(REPLACE "\n" " \\n" cmake_cxx_compiler_version ${cmake_cxx_compiler_version})
string(REPLACE "\n" " \\n" COMPILER_INFO ${COMPILER_INFO})

########################################################################
# Install directories
########################################################################
include(GNUInstallDirs)

# Install our Cmake modules into $prefix/lib/cmake/gnuradio
# See "Package Configuration Files" on page:
#    http://www.cmake.org/Wiki/CMake/Tutorials/Packaging

if(NOT CMAKE_MODULES_DIR)
    set(CMAKE_MODULES_DIR ${CMAKE_INSTALL_LIBDIR}/cmake)
endif(NOT CMAKE_MODULES_DIR)

# cmake-format: off
SET(GR_RUNTIME_DIR     ${CMAKE_INSTALL_BINDIR} CACHE PATH "Path to install all binaries")
SET(GR_LIBRARY_DIR     ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Path to install libraries")
SET(GR_INCLUDE_DIR     ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Path to install header files")
SET(GR_CMAKE_DIR       ${CMAKE_MODULES_DIR}/gnuradio)
SET(GR_DATA_DIR        ${CMAKE_INSTALL_DATADIR} CACHE PATH "Base location for data")
SET(GR_PKG_DATA_DIR    ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package data")
SET(GR_DOC_DIR         ${GR_DATA_DIR}/doc CACHE PATH "Path to install documentation")
SET(GR_PKG_DOC_DIR     ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME}-${DOCVER} CACHE PATH "Path to install package docs")
SET(GR_MAN_DIR         ${CMAKE_INSTALL_MANDIR} CACHE PATH "Path to install man pages")
SET(GR_LIBEXEC_DIR     ${CMAKE_INSTALL_LIBEXECDIR} CACHE PATH "Path to install libexec files")
SET(GR_PKG_LIBEXEC_DIR ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package libexec files")
SET(GRC_BLOCKS_DIR     ${GR_PKG_DATA_DIR}/grc/blocks CACHE PATH "Path to install GRC blocks")
SET(GR_THEMES_DIR      ${GR_PKG_DATA_DIR}/themes CACHE PATH "Path to install QTGUI themes")
SET(GRC_EXAMPLES_DIR   ${GR_PKG_DATA_DIR}/examples CACHE PATH "Path to install GRC example flowgraphs")
set(GR_CONF_DIR        ${CMAKE_INSTALL_SYSCONFDIR} CACHE PATH "Path to install config files")
set(GR_PKG_CONF_DIR    ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install package configs")
set(GR_PREFSDIR        ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install preference files")

if(DEFINED CACHE{SYSCONFDIR} AND NOT GR_IGNORE_SYSCONFDIR_SET)
    # If you need to ignore this error, set GR_IGNORE_SYSCONFDIR_SET
    message(FATAL_ERROR
        "SYSCONFDIR cache variable has been removed. Use GR_CONF_DIR or CMAKE_INSTALL_SYSCONFDIR instead."
    )
endif()
# cmake-format: on


option(ENABLE_PERFORMANCE_COUNTERS "Enable block performance counters" ON)
if(ENABLE_PERFORMANCE_COUNTERS)
    message(STATUS "ADDING PERF COUNTERS")
    set(GR_PERFORMANCE_COUNTERS True)
else(ENABLE_PERFORMANCE_COUNTERS)
    set(GR_PERFORMANCE_COUNTERS False)
    message(STATUS "NO PERF COUNTERS")
endif(ENABLE_PERFORMANCE_COUNTERS)

string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "/usr" isusr)
string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "/usr/local" isusrlocal)
string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "$ENV{HOME}/.local" ishomelocal)

option(ENABLE_BASH_COMPLETIONS "Enable shell completions for bash" ON)
if(ENABLE_BASH_COMPLETIONS)
    if(NOT DEFINED GR_BASH_COMPLETIONS_DIR)
        if(isusr OR isusrlocal OR ishomelocal)
            set(GR_BASH_COMPLETIONS_DIR
                ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
                CACHE PATH "Path to install bash completions")
        else(isusr OR isusrlocal OR ishomelocal)
            message(WARNING "GR_BASH_COMPLETIONS_DIR is not defined")
            set(ENABLE_BASH_COMPLETIONS OFF)
        endif(isusr OR isusrlocal OR ishomelocal)
    endif(NOT DEFINED GR_BASH_COMPLETIONS_DIR)
endif(ENABLE_BASH_COMPLETIONS)

option(ENABLE_ZSH_COMPLETIONS "Enable shell completions for zsh" ON)
if(ENABLE_ZSH_COMPLETIONS)
    if(NOT DEFINED GR_ZSH_COMPLETIONS_DIR)
        if(isusr OR isusrlocal)
            set(GR_ZSH_COMPLETIONS_DIR
                ${CMAKE_INSTALL_DATADIR}/zsh/site-functions
                CACHE PATH "Path to install zsh completions")
        else(isusr OR isusrlocal)
            message(WARNING "GR_ZSH_COMPLETIONS_DIR is not defined")
            set(ENABLE_ZSH_COMPLETIONS OFF)
        endif(isusr OR isusrlocal)
    endif(NOT DEFINED GR_ZSH_COMPLETIONS_DIR)
endif(ENABLE_ZSH_COMPLETIONS)

option(ENABLE_FISH_COMPLETIONS "Enable shell completions for fish" ON)
if(ENABLE_FISH_COMPLETIONS)
    if(NOT DEFINED GR_FISH_COMPLETIONS_DIR)
        if(isusr OR ishomelocal)
            set(GR_FISH_COMPLETIONS_DIR
                ${CMAKE_INSTALL_DATADIR}/fish/vendor_completions.d
                CACHE PATH "Path to install fish completions")
        else(isusr OR ishomelocal)
            message(WARNING "GR_FISH_COMPLETIONS_DIR is not defined")
            set(ENABLE_FISH_COMPLETIONS OFF)
        endif(isusr OR ishomelocal)
    endif(NOT DEFINED GR_FISH_COMPLETIONS_DIR)
endif(ENABLE_FISH_COMPLETIONS)

########################################################################
# Variables replaced when configuring the package config files
########################################################################
# cmake-format: off
file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}"           prefix)
file(TO_CMAKE_PATH "\${prefix}"                        exec_prefix)
file(TO_CMAKE_PATH "\${exec_prefix}/${GR_LIBRARY_DIR}" libdir)
file(TO_CMAKE_PATH "\${prefix}/${GR_INCLUDE_DIR}"      includedir)
# cmake-format: on

if(CMAKE_VERSION VERSION_LESS 3.20)
    set(dir SYSCONFIGDIR)
endif()
GNUInstallDirs_get_absolute_install_dir(GR_CONF_DIR_FULL GR_CONF_DIR SYSCONFIGDIR)
GNUInstallDirs_get_absolute_install_dir(GR_PREFSDIR_FULL GR_PREFSDIR SYSCONFIGDIR)

if(WIN32)
    file(RELATIVE_PATH prefix_relative_to_lib "${prefix}/${GR_RUNTIME_DIR}" "${prefix}")
else(WIN32)
    file(RELATIVE_PATH prefix_relative_to_lib "${prefix}/${GR_LIBRARY_DIR}" "${prefix}")
endif(WIN32)
if(NOT GR_BUILD_INSTALLER)
    file(RELATIVE_PATH GR_CONF_DIR_relative_to_prefix "${prefix}" "${GR_CONF_DIR_FULL}")
    file(RELATIVE_PATH GR_PREFSDIR_relative_to_prefix "${prefix}" "${GR_PREFSDIR_FULL}")
else()
    set(GR_CONF_DIR_relative_to_prefix "${GR_CONF_DIR}")
    set(GR_PREFSDIR_relative_to_prefix "${GR_PREFSDIR}")
endif()

########################################################################
# On Apple only, set install name and use rpath correctly, if not already set
########################################################################
if(APPLE)
    cmake_policy(SET CMP0068 NEW)
    set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/release/resources/icons/gnuradio_logo_icon-square.ico")
    if(NOT CMAKE_INSTALL_NAME_DIR AND NOT GR_BUILD_INSTALLER)
        set(CMAKE_INSTALL_NAME_DIR
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR}
            CACHE PATH "Library Install Name Destination Directory" )
    endif(NOT CMAKE_INSTALL_NAME_DIR)
    if(NOT CMAKE_INSTALL_RPATH)
        set(CMAKE_INSTALL_RPATH
            "@loader_path;@loader_path/../lib;@loader_path/../bin;@loader_path/..;@loader_path/../../../..;@loader_path/../../../../..;${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR};${CMAKE_INSTALL_PREFIX}/bin"
            CACHE PATH "Library Install RPath" FORCE)
    endif(NOT CMAKE_INSTALL_RPATH)
    if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
        set(GR_USE_INSTALL_RPATH ON)
        if(GR_BUILD_INSTALLER)
            set(GR_USE_INSTALL_RPATH OFF)
            set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
        endif()
        set(CMAKE_BUILD_WITH_INSTALL_RPATH
            ${GR_USE_INSTALL_RPATH} CACHE BOOL "Do Build Using Library Install RPath" FORCE)
    endif(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
endif(APPLE)

########################################################################
# Create uninstall target
########################################################################
configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)

add_custom_target(uninstall ${CMAKE_COMMAND} -P
                            ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

########################################################################
# Enable python component
########################################################################
include(GrPython)
gr_python_check_module(
    DESC "packaging"
    MODULE packaging
    VAR PACKAGING_FOUND)
if(NOT PACKAGING_FOUND)
    message(STATUS "")
    message(STATUS "***************************** WARNING!!! *******")
    message(STATUS "The python package packaging is missing")
    message(STATUS "This package is required for python support")
    message(STATUS "Please install python3-packaging")
    message(STATUS "or run pip3 install packaging")
    message(STATUS "************************************************")
endif()
gr_python_check_module(
    DESC "numpy"
    MODULE numpy
    CHECK "LooseVersion(numpy.__version__) >= LooseVersion('${GR_NUMPY_MIN_VERSION}')"
    VAR NUMPY_FOUND)
# Needed for automatic regeneration of some bindings
gr_python_check_module(
    DESC "pygccxml"
    MODULE pygccxml
    CHECK "LooseVersion(pygccxml.__version__) >= LooseVersion('${GR_PYGCCXML_MIN_VERSION}')"
    VAR PYGCCXML_FOUND)
if(NOT PYGCCXML_FOUND)
    message(STATUS "")
    message(STATUS "***************************** WARNING!!! *************************")
    message(STATUS "pygccxml is highly recommended for using gr_modtool")
    message(STATUS "and is either not present or below the minimum version "
                   ${GR_PYGCCXML_MIN_VERSION})
    message(STATUS "Only trivial bindings will be generated using gr_modtool bind ")
    message(STATUS "******************************************************************")
endif()

gr_find_package(pybind11)
if(pybind11_FOUND)
    if(${pybind11_VERSION} VERSION_LESS ${PYBIND11_MIN_VERSION})
        message(WARNING "pybind11 version ${pybind11_VERSION} < ${PYBIND11_MIN_VERSION}")
        set(pybind11_FOUND False)
    endif()
endif()

gr_register_component(
    NAME "python-support"
    VAR ENABLE_PYTHON
    REQUIRED_TPLS PYTHONLIBS_FOUND PACKAGING_FOUND pybind11_FOUND NUMPY_FOUND
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
    include(CodeCoverage)
    setup_target_for_coverage(coverage "ctest || exit 0" coverage)
endif()

########################################################################
# Enable/disable examples
########################################################################
option(ENABLE_EXAMPLES "Enable examples" ON)

########################################################################
# Detect and configure VOLK
########################################################################
message(STATUS "")
message(STATUS "Configuring VOLK support...")
# If not using conan/vcpkg to find deps, this is a no-op
# this suppresses the conan/vcpkg from volk's underlying find_package
# call to gmp
set(gmp_FIND_QUIETLY)
gr_find_package(Volk)
if(Volk_FOUND)
    message(STATUS "  Found VOLK:")
    message(STATUS "  * Version: ${Volk_VERSION}")
    message(STATUS "  * Libraries: ${VOLK_LIBRARIES}")
    message(STATUS "  * Includes: ${VOLK_INCLUDE_DIRS}")
    if("${Volk_VERSION}" STREQUAL "")
        message(WARNING "Empty VOLK version string. Assuming compatibility. Good luck!")
    else()
        if("${Volk_VERSION}" VERSION_LESS ${VOLK_MIN_VERSION})
            message(FATAL_ERROR "VOLK version ${Volk_VERSION} < ${VOLK_MIN_VERSION}")
        endif()
    endif()
else()
    message(WARNING "VOLK not found.")
endif()

########################################################################
# Setup Native Capabilities Flag
########################################################################
option(ENABLE_NATIVE "Enable native build optimizations" OFF)
if(UNIX)
    if(ENABLE_NATIVE)
        message(
            STATUS
                "Found GNU Radio native optimization flag.  Setting native CPU optimization flags."
        )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -ftree-vectorize")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -ftree-vectorize")
    else(ENABLE_NATIVE)
        message(
            STATUS "Not using additional GNU Radio native architecture optimizations.")
    endif(ENABLE_NATIVE)
endif(UNIX)

########################################################################
# Setup Installer Build and Options
########################################################################
option(GR_BUILD_INSTALLER "Build native GNURadio installer" OFF)

########################################################################
# Disable complex math NaN/INFO range checking for performance
########################################################################
check_cxx_compiler_flag(-fcx-limited-range HAVE_CX_LIMITED_RANGE)
if(HAVE_CX_LIMITED_RANGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcx-limited-range")
    check_cxx_compiler_flag(-Wno-unused-command-line-argument
                            HAVE_WNO_UNUSED_CMD_LINE_ARG)
    if(HAVE_WNO_UNUSED_CMD_LINE_ARG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
    endif(HAVE_WNO_UNUSED_CMD_LINE_ARG)
endif(HAVE_CX_LIMITED_RANGE)

########################################################################
# Distribute the README file
########################################################################
install(FILES README.md CONTRIBUTING.md CHANGELOG.md DESTINATION ${GR_PKG_DOC_DIR})

install(
    FILES .clang-format
    RENAME clang-format.conf
    DESTINATION ${GR_PKG_DATA_DIR})
install(FILES .cmake-format.py DESTINATION ${GR_PKG_DATA_DIR})

########################################################################
# The following dependency libraries are needed by all gr modules:
########################################################################
list(APPEND GR_TEST_PYTHON_DIRS ${PROJECT_BINARY_DIR}/gnuradio-runtime/python
     ${PROJECT_SOURCE_DIR}/gnuradio-runtime/python)

# Note that above we put the binary gnuradio-runtime/python directory
# before the source directory. This is due to a quirk with ControlPort
# and how slice generates files and names. We want the QA and
# installed code to import the same names, so we have to grab from the
# binary directory first.

# gnuradio-runtime/include/gnuradio/block.h needs a define to tell it which
# multiprecision arithmetic library header to include
gr_find_package(MPLIB)

########################################################################
# Post-Install tasks are tasks that are usually not executed during
# install, but for source builds, that's actually more convenient.
########################################################################
gr_register_component(
    NAME "post-install"
    VAR ENABLE_POSTINSTALL
)

########################################################################
# Add subdirectories (in order of deps)
########################################################################
add_subdirectory(docs)
add_subdirectory(gnuradio-runtime)
if(ENABLE_PYTHON)
    # Setup portable path variables for cross-platform installations
    include(setupGrPkgPath)
    if(WIN32)
        # Gtk requires special configuration due to runtime
        # dll loading on Windows
        include(setup-gtk)
    endif()
    add_subdirectory(grc)
endif()
add_subdirectory(gr-blocks)
add_subdirectory(gr-fec)
add_subdirectory(gr-fft)
add_subdirectory(gr-filter)
add_subdirectory(gr-analog)
add_subdirectory(gr-digital)
add_subdirectory(gr-dtv)
add_subdirectory(gr-audio)
add_subdirectory(gr-channels)
add_subdirectory(gr-pdu)
add_subdirectory(gr-iio)
add_subdirectory(gr-qtgui)
add_subdirectory(gr-trellis)
add_subdirectory(gr-uhd)
if(ENABLE_PYTHON)
    add_subdirectory(gr-utils)
endif()
add_subdirectory(gr-video-sdl)
add_subdirectory(gr-vocoder)
add_subdirectory(gr-wavelet)
add_subdirectory(gr-zeromq)
add_subdirectory(gr-network)
add_subdirectory(gr-soapy)

# Defining GR_CTRLPORT for gnuradio/config.h
if(ENABLE_GR_CTRLPORT)
    set(GR_CTRLPORT True)

    if(CTRLPORT_BACKENDS GREATER 0)
        set(GR_RPCSERVER_ENABLED True)

        if(ENABLE_CTRLPORT_THRIFT)
            set(GR_RPCSERVER_THRIFT True)
        endif(ENABLE_CTRLPORT_THRIFT)
    endif(CTRLPORT_BACKENDS GREATER 0)
endif(ENABLE_GR_CTRLPORT)

# Install all other cmake files into same directory
file(GLOB cmake_others "cmake/Modules/*.cmake")
list(REMOVE_ITEM cmake_others "${PROJECT_SOURCE_DIR}/cmake/Modules/FindGnuradio.cmake")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/Modules/GnuradioConfig.cmake.in
    ${PROJECT_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
    INSTALL_DESTINATION ${CMAKE_MODULES_DIR}/gnuradio)

configure_file(${PROJECT_SOURCE_DIR}/cmake/Modules/GnuradioConfigVersion.cmake.in
               ${PROJECT_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake @ONLY)

set(cmake_configs ${PROJECT_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
                  ${PROJECT_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake)

install(FILES ${cmake_configs} ${cmake_others} DESTINATION ${CMAKE_MODULES_DIR}/gnuradio)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/grc/grc.conf
     "enabled_components = ${_gr_enabled_components}")

########################################################################
# Print summary
########################################################################
gr_print_component_summary()
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

# Create a config.h with some definitions to export to other projects.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/include/gnuradio/config.h)

# Re-generate the constants file, now that we actually know which components will be enabled.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime/lib/constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/lib/constants.cc ESCAPE_QUOTES @ONLY)

# Install config.h in include/gnuradio
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/include/gnuradio/config.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio)

# Install rules are needed for any deployment, incl. make install
GR_GENERATE_INSTALL()

# Enable rules for build binary installer (for Windows, Mac)
if(GR_BUILD_INSTALLER)
    include(cmake/Modules/GrCPack.cmake)
endif(GR_BUILD_INSTALLER)