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

########################################################################
# Project setup
########################################################################
# Make sure this version matches ${GR_CMAKE_MIN_VERSION} (a variable can't be
# used here).
cmake_minimum_required(VERSION 3.8)
project(gnuradio CXX C)
enable_testing()
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(GrBuildTypes)

# Select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
GR_CHECK_BUILD_TYPE(${CMAKE_BUILD_TYPE})
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}.")

# Set the build date
string(TIMESTAMP BUILD_DATE "%a, %d %b %Y %H:%M:%SZ" UTC)
string(TIMESTAMP BUILD_DATE_SHORT "%Y-%m-%d" UTC)
message(STATUS "Build date set to ${BUILD_DATE}.")

include(GrComponent)
########################################################################
# Setup Boost for global use (within this build)
# Do this before enabling testing support, as it depends
# on unit_test_framework
########################################################################
include(GrBoost)
GR_REGISTER_COMPONENT("testing-support" ENABLE_TESTING
         Boost_UNIT_TEST_FRAMEWORK_FOUND )

# Set the version information here
SET(VERSION_MAJOR 3)
SET(VERSION_API   10)
SET(VERSION_ABI   0)
SET(VERSION_PATCH git)
include(GrVersion) #setup version info

# Minimum dependency versions for central dependencies:

set(GR_BOOST_MIN_VERSION "1.65")      ## Version in Ubuntu 18.04LTS
set(GR_CMAKE_MIN_VERSION "3.10.2")    ## Version in Ubuntu 18.04LTS
set(GR_MAKO_MIN_VERSION "1.0.7")      ## debian buster, 18.04LTS
set(GR_PYTHON_MIN_VERSION "3.6.5")    ## Version in Ubuntu 18.04LTS
set(GR_NUMPY_MIN_VERSION "1.13.3")    ## Version in Ubuntu 18.04LTS
set(GCC_MIN_VERSION "8.3.0")          ## debian buster
set(CLANG_MIN_VERSION "11.0.0")       ## debian bullseye, Fedora 33
set(APPLECLANG_MIN_VERSION "1100")    ## same as clang 11.0.0, in Xcode11
set(MSVC_MIN_VERSION "1914")          ## VS2017 15.7, for full-ish C++17 support
set(VOLK_MIN_VERSION "2.4.1")         ## first version with CPU features
set(PYBIND11_MIN_VERSION "2.4")       # pybind11 sets versions like 2.4.dev4, which compares < 2.4.3
set(GR_THRIFT_MIN_VERSION "0.13")

# Enable generation of compile_commands.json for code completion engines
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set wiki block docs url prefix used in grc-docs.conf (block name gets appended to end of this string)
set(GRC_DOCS_URL_PREFIX "https://wiki.gnuradio.org/index.php/")

########################################################################
# Compiler version setup
########################################################################
# Append -O2 optimization flag for Debug builds (Not on MSVC since conflicts with RTC1 flag)
IF (NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2")
ENDIF()

# Check compiler version against our minimum
IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    IF(DEFINED CMAKE_CXX_COMPILER_VERSION)
        IF(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS ${GCC_MIN_VERSION})
            MESSAGE(WARNING "\nThe compiler selected to build GNU Radio (GCC version ${CMAKE_CXX_COMPILER_VERSION} : ${CMAKE_CXX_COMPILER}) is older than that officially supported (${GCC_MIN_VERSION} minimum). This build may or not work. We highly recommend using a more recent GCC version.")
        ENDIF()
    ELSE()
        MESSAGE(WARNING "\nCannot determine the version of the compiler selected to build GNU Radio (GCC : ${CMAKE_CXX_COMPILER}). This build may or not work. We highly recommend using GCC version ${GCC_MIN_VERSION} or more recent.")
    ENDIF()
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    EXECUTE_PROCESS(COMMAND
        ${CMAKE_CXX_COMPILER} -v
        RESULT_VARIABLE _res ERROR_VARIABLE _err
        ERROR_STRIP_TRAILING_WHITESPACE)
    IF(${_res} STREQUAL "0")
        # output is in error stream
        STRING(REGEX MATCH "^Apple.*" IS_APPLE ${_err})
        IF("${IS_APPLE}" STREQUAL "")
            SET(MIN_VERSION ${CLANG_MIN_VERSION})
            SET(APPLE_STR "")
            # retrieve the compiler's version from it
            STRING(REGEX MATCH "clang version [0-9.]+" CLANG_OTHER_VERSION ${_err})
            STRING(REGEX MATCH "[0-9.]+" CLANG_VERSION ${CLANG_OTHER_VERSION})
        ELSE()
            SET(MIN_VERSION ${APPLECLANG_MIN_VERSION})
            SET(APPLE_STR "Apple ")
            # retrieve the compiler's version from it
            STRING(REGEX MATCH "(clang-[0-9.]+)" CLANG_APPLE_VERSION ${_err})
            STRING(REGEX MATCH "[0-9.]+" CLANG_VERSION ${CLANG_APPLE_VERSION})
        ENDIF()
        IF(${CLANG_VERSION} VERSION_LESS "${MIN_VERSION}")
            MESSAGE(WARNING "\nThe compiler selected to build GNU Radio (${APPLE_STR}Clang version ${CLANG_VERSION} : ${CMAKE_CXX_COMPILER}) is older than that officially supported (${MIN_VERSION} minimum). This build may or not work. We highly recommend using Apple Clang version ${APPLECLANG_MIN_VERSION} or more recent, or Clang version ${CLANG_MIN_VERSION} or more recent.")
        ENDIF()
    ELSE()
        MESSAGE(WARNING "\nCannot determine the version of the compiler selected to build GNU Radio (${APPLE_STR}Clang : ${CMAKE_CXX_COMPILER}). This build may or not work. We highly recommend using Apple Clang version ${APPLECLANG_MIN_VERSION} or more recent, or Clang version ${CLANG_MIN_VERSION} or more recent.")
    ENDIF()
ELSE()
    MESSAGE(status "Skipping compiler version check.")
ENDIF()

# Configure C++ standard if not externally specified (will actually be
# set after CppUnit check below). Use the variable CMAKE_CXX_STANDARD
# since it will actually be used for this purposes starting in CMake 3.1.

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

########################################################################
# Environment setup
########################################################################
IF(NOT DEFINED BOOST_ROOT)
    SET(BOOST_ROOT "${CMAKE_INSTALL_PREFIX}")
ENDIF()

########################################################################
# Import executables from a native build (for cross compiling)
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
if(IMPORT_EXECUTABLES)
    include(${IMPORT_EXECUTABLES})
endif(IMPORT_EXECUTABLES)

#set file that the native build will fill with exports
SET(EXPORT_FILE ${CMAKE_BINARY_DIR}/ImportExecutables.cmake)
file(WRITE ${EXPORT_FILE}) #blank the file (subdirs will append)

########################################################################
# Incorporate CMake function/macros overloading.
########################################################################

include(CMakeOverloads)

########################################################################
# Compiler specific setup
########################################################################
include(GrMiscUtils) #compiler flag check

include(TestBigEndian)
TEST_BIG_ENDIAN(GR_IS_BIG_ENDIAN)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT WIN32)
        #http://gcc.gnu.org/wiki/Visibility
        GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
    endif(NOT WIN32)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wsign-compare HAVE_WARN_SIGN_COMPARE)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wall HAVE_WARN_ALL)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wno-uninitialized HAVE_WARN_NO_UNINITIALIZED)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

if(MSVC)
    include_directories(${CMAKE_SOURCE_DIR}/cmake/msvc) #missing headers
    add_compile_options(/MP) #build with multiple processors
    add_compile_options(/bigobj) #allow for larger object files
endif(MSVC)

# Record Compiler Info for record
STRING(TOUPPER ${CMAKE_BUILD_TYPE} GRCBTU)
SET(COMPILER_INFO "")
IF(MSVC)
    IF(MSVC_VERSION LESS "${MSVC_MIN_VERSION}")
        MESSAGE(FATAL_ERROR "MSVC Versions < minimum version ${MSVC_MIN_VERSION}")
    ENDIF()
    SET(cmake_c_compiler_version ${MSVC_VERSION})
    SET(cmake_cxx_compiler_version ${MSVC_VERSION})
ELSE()
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version
            OUTPUT_VARIABLE cmake_c_compiler_version)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
            OUTPUT_VARIABLE cmake_cxx_compiler_version)
ENDIF(MSVC)
SET(COMPILER_INFO "${CMAKE_C_COMPILER}:::${CMAKE_C_FLAGS_${GRCBTU}} ${CMAKE_C_FLAGS}\n${CMAKE_CXX_COMPILER}:::${CMAKE_CXX_FLAGS_${GRCBTU}} ${CMAKE_CXX_FLAGS}\n" )

# Convert to a C string to compile and display properly
string(STRIP "${cmake_c_compiler_version}" cmake_c_compiler_version)
string(STRIP "${cmake_cxx_compiler_version}" cmake_cxx_compiler_version)
string(STRIP ${COMPILER_INFO} COMPILER_INFO)
MESSAGE(STATUS "Compiler Version: ${cmake_c_compiler_version}")
MESSAGE(STATUS "Compiler Flags: ${COMPILER_INFO}")
string(REPLACE "\n" " \\n" cmake_c_compiler_version ${cmake_c_compiler_version})
string(REPLACE "\n" " \\n" cmake_cxx_compiler_version ${cmake_cxx_compiler_version})
string(REPLACE "\n" " \\n" COMPILER_INFO ${COMPILER_INFO})

########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX

# Install our Cmake modules into $prefix/lib/cmake/gnuradio
# See "Package Configuration Files" on page:
#    http://www.cmake.org/Wiki/CMake/Tutorials/Packaging

if(NOT CMAKE_MODULES_DIR)
  SET(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

SET(GR_RUNTIME_DIR     bin CACHE PATH "Path to install all binaries")
SET(GR_LIBRARY_DIR     lib${LIB_SUFFIX} CACHE PATH "Path to install libraries")
SET(GR_INCLUDE_DIR     include CACHE PATH "Path to install header files")
SET(GR_CMAKE_DIR       ${CMAKE_MODULES_DIR}/gnuradio)
SET(GR_DATA_DIR        share CACHE PATH "Base location for data")
SET(GR_PKG_DATA_DIR    ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package data")
SET(GR_DOC_DIR         ${GR_DATA_DIR}/doc CACHE PATH "Path to install documentation")
SET(GR_PKG_DOC_DIR     ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME}-${DOCVER} CACHE PATH "Path to install package docs")
SET(GR_MAN_DIR         ${GR_DATA_DIR}/man CACHE PATH "Path to install man pages")
SET(GR_LIBEXEC_DIR     libexec CACHE PATH "Path to install libexec files")
SET(GR_PKG_LIBEXEC_DIR ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package libexec files")
SET(GRC_BLOCKS_DIR     ${GR_PKG_DATA_DIR}/grc/blocks CACHE PATH "Path to install GRC blocks")
SET(GR_THEMES_DIR      ${GR_PKG_DATA_DIR}/themes CACHE PATH "Path to install QTGUI themes")

# Set location of config/prefs files in /etc
# Special exception if prefix is /usr so we don't make a /usr/etc.
SET(GR_CONF_DIR etc CACHE PATH "Path to install config files")
string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "/usr" isusr)
if(isusr)
  SET(SYSCONFDIR "/${GR_CONF_DIR}" CACHE PATH "System configuration directory")
else(isusr)
  SET(SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/${GR_CONF_DIR}" CACHE PATH "System configuration directory" FORCE)
endif(isusr)

SET(GR_PKG_CONF_DIR ${SYSCONFDIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install package configs")
SET(GR_PREFSDIR     ${SYSCONFDIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install preference files")

OPTION(ENABLE_PERFORMANCE_COUNTERS "Enable block performance counters" ON)
if(ENABLE_PERFORMANCE_COUNTERS)
  message(STATUS "ADDING PERF COUNTERS")
  SET(GR_PERFORMANCE_COUNTERS True)
else(ENABLE_PERFORMANCE_COUNTERS)
  SET(GR_PERFORMANCE_COUNTERS False)
  message(STATUS "NO PERF COUNTERS")
endif(ENABLE_PERFORMANCE_COUNTERS)

########################################################################
# Variables replaced when configuring the package config files
########################################################################
file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}"           prefix)
file(TO_CMAKE_PATH "\${prefix}"                        exec_prefix)
file(TO_CMAKE_PATH "\${exec_prefix}/${GR_LIBRARY_DIR}" libdir)
file(TO_CMAKE_PATH "\${prefix}/${GR_INCLUDE_DIR}"      includedir)
file(TO_CMAKE_PATH "${SYSCONFDIR}"                     SYSCONFDIR)
file(TO_CMAKE_PATH "${GR_PREFSDIR}"                    GR_PREFSDIR)

if(WIN32)
    file(RELATIVE_PATH prefix_relative_to_lib "${prefix}/${GR_RUNTIME_DIR}" "${prefix}")
else(WIN32)
    file(RELATIVE_PATH prefix_relative_to_lib "${prefix}/${GR_LIBRARY_DIR}" "${prefix}")
endif(WIN32)
file(RELATIVE_PATH SYSCONFDIR_relative_to_prefix "${prefix}" "${SYSCONFDIR}")
file(RELATIVE_PATH GR_PREFSDIR_relative_to_prefix "${prefix}" "${GR_PREFSDIR}")

########################################################################
# On Apple only, set install name and use rpath correctly, if not already set
########################################################################
if(APPLE)
    if(NOT CMAKE_INSTALL_NAME_DIR)
        SET(CMAKE_INSTALL_NAME_DIR
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install Name Destination Directory" FORCE)
    endif(NOT CMAKE_INSTALL_NAME_DIR)
    if(NOT CMAKE_INSTALL_RPATH)
        SET(CMAKE_INSTALL_RPATH
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install RPath" FORCE)
    endif(NOT CMAKE_INSTALL_RPATH)
    if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE
            BOOL "Do Build Using Library Install RPath" FORCE)
    endif(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
endif(APPLE)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Enable python component
########################################################################
include(GrPython)
GR_PYTHON_CHECK_MODULE(
    "numpy"
    numpy
    "LooseVersion(numpy.__version__) >= LooseVersion('${GR_NUMPY_MIN_VERSION}')"
    NUMPY_FOUND)
# Needed for automatic regeneration of some bindings
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )


find_package(pybind11 REQUIRED)
IF(${pybind11_VERSION} VERSION_LESS ${PYBIND11_MIN_VERSION})
    message(FATAL_ERROR "pybind11 version ${pybind11_VERSION} < ${PYBIND11_MIN_VERSION}")
ENDIF()

execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c
    "try:\n import numpy\n import os\n inc_path = numpy.get_include()\n if os.path.exists(os.path.join(inc_path, 'numpy', 'arrayobject.h')):\n  print(inc_path, end='')\nexcept:\n pass"
    OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_DIR)
# format path in CMake-style for consistency with other path variables
# (a consistent style helps conda builds by using the same path separators)
file(TO_CMAKE_PATH "${PYTHON_NUMPY_INCLUDE_DIR}" PYTHON_NUMPY_INCLUDE_DIR)

include(GrComponent)
GR_REGISTER_COMPONENT("python-support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    pybind11_FOUND
    NUMPY_FOUND
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
  include(CodeCoverage)
  setup_target_for_coverage(coverage "ctest || exit 0" coverage)
endif()

########################################################################
# Enable/disable examples
########################################################################
OPTION(ENABLE_EXAMPLES "Enable examples" ON)

########################################################################
# Detect and configure VOLK
########################################################################
message(STATUS "")
message(STATUS "Configuring VOLK support...")
find_package(Volk REQUIRED)
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

########################################################################
# Configure Log4CPP
########################################################################
find_package(LOG4CPP REQUIRED)

########################################################################
# Setup Native Capabilities Flag
########################################################################
option(ENABLE_NATIVE "Enable native build optimizations" OFF)
IF(UNIX)
    IF (ENABLE_NATIVE)
        MESSAGE(STATUS "Found GNU Radio native optimization flag.  Setting native CPU optimization flags.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -ftree-vectorize")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -ftree-vectorize")
    ELSE (ENABLE_NATIVE)
        MESSAGE(STATUS "Not using additional GNU Radio native architecture optimizations.")
    ENDIF (ENABLE_NATIVE)
ENDIF(UNIX)

########################################################################
# Disable complex math NaN/INFO range checking for performance
########################################################################
check_cxx_compiler_flag(-fcx-limited-range HAVE_CX_LIMITED_RANGE)
if(HAVE_CX_LIMITED_RANGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcx-limited-range")
endif(HAVE_CX_LIMITED_RANGE)

########################################################################
# Distribute the README file
########################################################################
install(
    FILES README.md CONTRIBUTING.md CHANGELOG.md
    DESTINATION ${GR_PKG_DOC_DIR}
)

install(
    FILES .clang-format
    RENAME clang-format.conf
    DESTINATION ${GR_PKG_DATA_DIR}
)

########################################################################
# The following dependency libraries are needed by all gr modules:
########################################################################
list(APPEND GR_TEST_PYTHON_DIRS
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/python
)



# Note that above we put the binary gnuradio-runtime/python directory
# before the source directory. This is due to a quirk with ControlPort
# and how slice generates files and names. We want the QA and
# installed code to import the same names, so we have to grab from the
# binary directory first.

# gnuradio-runtime/include/gnuradio/block.h needs a define to tell it which
# multiprecision arithmetic library header to include
include(FindPkgConfig)
find_package(MPLIB)

########################################################################
# Post-Install tasks are tasks that are usually not executed during
# install, but for source builds, that's actually more convenient.
########################################################################
GR_REGISTER_COMPONENT("post-install" ENABLE_POSTINSTALL)

########################################################################
# Add subdirectories (in order of deps)
########################################################################
add_subdirectory(docs)
add_subdirectory(gnuradio-runtime)
add_subdirectory(grc)
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
add_subdirectory(gr-qtgui)
add_subdirectory(gr-trellis)
add_subdirectory(gr-uhd)
add_subdirectory(gr-utils)
add_subdirectory(gr-video-sdl)
add_subdirectory(gr-vocoder)
add_subdirectory(gr-wavelet)
add_subdirectory(gr-zeromq)
add_subdirectory(gr-network)

# Defining GR_CTRLPORT for gnuradio/config.h
if(ENABLE_GR_CTRLPORT)
  SET(GR_CTRLPORT True)

  if(CTRLPORT_BACKENDS GREATER 0)
    SET(GR_RPCSERVER_ENABLED True)

    if(THRIFT_FOUND)
      SET(GR_RPCSERVER_THRIFT True)
    endif(THRIFT_FOUND)
  endif(CTRLPORT_BACKENDS GREATER 0)
endif(ENABLE_GR_CTRLPORT)


# Install all other cmake files into same directory
file(GLOB cmake_others "cmake/Modules/*.cmake")
list(REMOVE_ITEM cmake_others
    "${CMAKE_SOURCE_DIR}/cmake/Modules/FindGnuradio.cmake"
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/Modules/GnuradioConfig.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
  INSTALL_DESTINATION ${CMAKE_MODULES_DIR}/gnuradio
  )

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/Modules/GnuradioConfigVersion.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake
@ONLY)

SET(cmake_configs
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake
)


install(
  FILES ${cmake_configs} ${cmake_others}
  DESTINATION ${CMAKE_MODULES_DIR}/gnuradio
)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/grc/grc.conf "enabled_components = ${_gr_enabled_components}")

########################################################################
# Print summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

# Create a config.h with some definitions to export to other projects.
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/include/gnuradio/config.h
)

# Re-generate the constants file, now that we actually know which components will be enabled.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime/lib/constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/lib/constants.cc
    ESCAPE_QUOTES
@ONLY)

# Install config.h in include/gnuradio
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime/include/gnuradio/config.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
)
