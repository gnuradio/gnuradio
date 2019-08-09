# Copyright 2011-2012,2014,2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

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

# Set the version information here
SET(VERSION_MAJOR 3)
SET(VERSION_API   9)
SET(VERSION_ABI   0)
SET(VERSION_PATCH 0-git)
include(GrVersion) #setup version info

# Minimum dependency versions for central dependencies:
set(GR_BOOST_MIN_VERSION "1.53")
set(GR_SWIG_MIN_VERSION "3.0.8")
set(GR_CMAKE_MIN_VERSION "3.5.1")
set(GR_MAKO_MIN_VERSION "0.4.2")
set(GR_PYTHON_MIN_VERSION "2.7.6")
set(GR_PYTHON3_MIN_VERSION "3.6.5")
set(GCC_MIN_VERSION "4.8.4")
set(CLANG_MIN_VERSION "3.4.0")
set(APPLECLANG_MIN_VERSION "500")
set(MSVC_MIN_VERSION "1800")

# Enable generation of compile_commands.json for code completion engines
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set wiki block docs url prefix used in grc-docs.conf (block name gets appended to end of this string)
set(GRC_DOCS_URL_PREFIX "https://wiki.gnuradio.org/index.php/")

########################################################################
# Configure CMake policies
########################################################################
# This will suppress developer warnings during the cmake process that can occur
# if a newer cmake version than the minimum is used.
if(POLICY CMP0026)
    cmake_policy(SET CMP0026 NEW)
endif()
if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()
if(POLICY CMP0045)
    cmake_policy(SET CMP0045 NEW)
endif()
if(POLICY CMP0046)
    cmake_policy(SET CMP0046 NEW)
endif()

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

SET(CMAKE_C_EXTENSIONS OFF)
SET(CMAKE_CXX_EXTENSIONS OFF)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_STANDARD 11)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_STANDARD 11)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_CXX_STANDARD 11)
ELSE()
    message(WARNING "C++ standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_C_STANDARD 11)
ELSEIF(CMAKE_C_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_C_STANDARD 11)
ELSEIF(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_C_STANDARD 11)
ELSE()
    message(WARNING "C standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

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
    add_definitions(-D_USE_MATH_DEFINES) #enables math constants on all supported versions of MSVC
    add_definitions(-D_WIN32_WINNT=0x0502) #Minimum version: "Windows Server 2003 with SP1, Windows XP with SP2"
    add_definitions(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    add_definitions( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    add_definitions(-DHAVE_CONFIG_H)
    add_compile_options(/MP) #build with multiple processors
    add_compile_options(/bigobj) #allow for larger object files
endif(MSVC)

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DMS_WIN64)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(WIN32)

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
  add_definitions(-DGR_PERFORMANCE_COUNTERS)
else(ENABLE_PERFORMANCE_COUNTERS)
  SET(GR_PERFORMANCE_COUNTERS False)
  message(STATUS "NO PERF COUNTERS")
endif(ENABLE_PERFORMANCE_COUNTERS)

########################################################################
# Variables replaced when configuring the package config files
########################################################################
file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}"           prefix)
file(TO_NATIVE_PATH "\${prefix}"                        exec_prefix)
file(TO_NATIVE_PATH "\${exec_prefix}/${GR_LIBRARY_DIR}" libdir)
file(TO_NATIVE_PATH "\${prefix}/${GR_INCLUDE_DIR}"      includedir)
file(TO_NATIVE_PATH "${SYSCONFDIR}"                     SYSCONFDIR)
file(TO_NATIVE_PATH "${GR_PREFSDIR}"                    GR_PREFSDIR)

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
# Setup Boost for global use (within this build)
########################################################################
include(GrBoost)

########################################################################
# Enable python component
########################################################################
include(GrPython)
find_package(PythonLibs ${GR_PYTHON_MIN_VERSION})
GR_PYTHON_CHECK_MODULE("six - python 2 and 3 compatibility library" six "True" SIX_FOUND)
find_package(SWIG)

if(SWIG_FOUND)
    SET(SWIG_VERSION_CHECK FALSE)
    if("${SWIG_VERSION}" VERSION_EQUAL ${GR_SWIG_MIN_VERSION} OR
       "${SWIG_VERSION}" VERSION_GREATER ${GR_SWIG_MIN_VERSION})
        SET(SWIG_VERSION_CHECK TRUE)
    endif()
endif(SWIG_FOUND)

include(GrComponent)
GR_REGISTER_COMPONENT("python-support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    SWIG_FOUND
    SWIG_VERSION_CHECK
    SIX_FOUND
)

GR_REGISTER_COMPONENT("testing-support" ENABLE_TESTING
    Boost_FOUND
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Coverage")
  include(CodeCoverage)
  setup_target_for_coverage(coverage "ctest || exit 0" coverage)
endif()


########################################################################
# Setup volk as a subproject
########################################################################
message(STATUS "")
message(STATUS "Configuring VOLK support...")

OPTION(ENABLE_INTERNAL_VOLK "Enable internal VOLK only" ON)
UNSET(Volk_FOUND)
if(NOT ENABLE_INTERNAL_VOLK)
    find_package(Volk)
    set(GR_VOLK_LIB "Volk::volk")
    if(NOT Volk_FOUND)
        message(STATUS "  External VOLK not found; checking internal.")
    endif()
endif()
if(NOT Volk_FOUND)
    find_file(INTREE_VOLK_FOUND
        volk/volk_common.h
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/volk/include
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )

    if(NOT INTREE_VOLK_FOUND)
        message(STATUS "  VOLK submodule is not checked out.")
        message(STATUS "  To check out the VOLK submodule, use:")
        message(STATUS "    git pull --recurse-submodules=on")
        message(STATUS "    git submodule update --init")
        if(ENABLE_INTERNAL_VOLK)
            message(STATUS "  External VOLK disabled.")
        endif()
        message(STATUS "  Override with -DENABLE_INTERNAL_VOLK=ON/OFF")
        message(STATUS "")
        message(FATAL_ERROR "VOLK required but not found.")
    endif()

    add_subdirectory(volk)
    # if the above command returns, then VOLK is enabled

    include(GrComponent)
    GR_REGISTER_COMPONENT("volk" ENABLE_VOLK)

    SET(VOLK_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/volk/include
        ${CMAKE_CURRENT_BINARY_DIR}/volk/include
    )

    set(GR_VOLK_LIB "volk")

    SET(VOLK_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
    SET(VOLK_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
else()
    message(STATUS "  An external VOLK has been found and will be used for build.")
    SET(ENABLE_VOLK TRUE)

    get_filename_component(VOLK_INSTALL_LIBRARY_DIR "${VOLK_LIBRARIES}" DIRECTORY)
    SET(VOLK_INSTALL_INCLUDE_DIR ${VOLK_INCLUDE_DIRS})
endif(NOT Volk_FOUND)

message(STATUS "  Override with -DENABLE_INTERNAL_VOLK=ON/OFF")

# Handle logging
find_package(LOG4CPP REQUIRED)

########################################################################
# Distribute the README file
########################################################################
install(
    FILES README.md README.hacking CHANGELOG.md
    DESTINATION ${GR_PKG_DOC_DIR}
)

########################################################################
# The following dependency libraries are needed by all gr modules:
########################################################################
list(APPEND GR_TEST_PYTHON_DIRS
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/python
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/swig
)



# Note that above we put the binary gnuradio-runtime/python directory
# before the source directory. This is due to a quirk with ControlPort
# and how slice generates files and names. We want the QA and
# installed code to import the same names, so we have to grab from the
# binary directory first.

# gnuradio-runtime/include/gnuradio/block.h needs a define to tell it which
# multiprecision arithmetic library header to include
find_package(MPLIB)
add_definitions(${MPLIB_DEFINITIONS})

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
add_subdirectory(gr-qtgui)
add_subdirectory(gr-trellis)
add_subdirectory(gr-uhd)
add_subdirectory(gr-utils)
add_subdirectory(gr-video-sdl)
add_subdirectory(gr-vocoder)
add_subdirectory(gr-wavelet)
add_subdirectory(gr-zeromq)

# Defining GR_CTRLPORT for gnuradio/config.h
if(ENABLE_GR_CTRLPORT)
  SET(GR_CTRLPORT True)
  add_definitions(-DGR_CTRLPORT)

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

#Re-generate the constants file, now that we actually know which components will be enabled.
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
