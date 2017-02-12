# Copyright 2010-2016 Free Software Foundation, Inc.
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
cmake_minimum_required(VERSION 2.6)
project(gnuradio CXX C)
enable_testing()

#make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(GrBuildTypes)

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
GR_CHECK_BUILD_TYPE(${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}.")

# Set the version information here
set(VERSION_INFO_MAJOR_VERSION 3)
set(VERSION_INFO_API_COMPAT    7)
set(VERSION_INFO_MINOR_VERSION 11)
set(VERSION_INFO_MAINT_VERSION git)
include(GrVersion) #setup version info

# Append -O2 optimization flag for Debug builds (Not on MSVC since conflicts with RTC1 flag)
IF (NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2")
ENDIF()

# Set C/C++ standard for all targets
# NOTE: Starting with cmake v3.1 this should be used:
# set(CMAKE_C_STANDARD 90)
# set(CMAKE_CXX_STANDARD 98)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98")
ELSE()
    message(warning "C++ standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
ELSEIF(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
ELSEIF(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
ELSE()
    message(warning "C standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

# Set cmake policies.
# This will suppress developer warnings during the cmake process that can occur
# if a newer cmake version than the minimum is used.

if(POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
endif()
if(POLICY CMP0043)
    cmake_policy(SET CMP0043 OLD)
endif()
if(POLICY CMP0045)
    cmake_policy(SET CMP0045 OLD)
endif()
if(POLICY CMP0046)
    cmake_policy(SET CMP0046 OLD)
endif()

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
set(EXPORT_FILE ${CMAKE_BINARY_DIR}/ImportExecutables.cmake)
file(WRITE ${EXPORT_FILE}) #blank the file (subdirs will append)

########################################################################
# Compiler specific setup
########################################################################
include(GrMiscUtils) #compiler flag check

if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wsign-compare HAVE_WARN_SIGN_COMPARE)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wall HAVE_WARN_ALL)
    GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-Wno-uninitialized HAVE_WARN_NO_UNINITIALIZED)
endif(CMAKE_COMPILER_IS_GNUCXX)

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
    add_definitions(/MP) #build with multiple processors
    add_definitions(/bigobj) #allow for larger object files
endif(MSVC)

if(WIN32)
	add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

# Record Compiler Info for record
STRING(TOUPPER ${CMAKE_BUILD_TYPE} GRCBTU)
set(COMPILER_INFO "")
IF(MSVC)
    IF(MSVC90)   #Visual Studio 9
        SET(cmake_c_compiler_version "Microsoft Visual Studio 9.0")
        SET(cmake_cxx_compiler_version "Microsoft Visual Studio 9.0")
    ELSE(MSVC10) #Visual Studio 10
        SET(cmake_c_compiler_version "Microsoft Visual Studio 10.0")
        SET(cmake_cxx_compiler_version "Microsoft Visual Studio 10.0")
    ELSE(MSVC11) #Visual Studio 11
        SET(cmake_c_compiler_version "Microsoft Visual Studio 11.0")
        SET(cmake_cxx_compiler_version "Microsoft Visual Studio 11.0")
    ELSE(MSVC12) #Visual Studio 12
        SET(cmake_c_compiler_version "Microsoft Visual Studio 12.0")
        SET(cmake_cxx_compiler_version "Microsoft Visual Studio 12.0")
    ELSE(MSVC14) #Visual Studio 14
        SET(cmake_c_compiler_version "Microsoft Visual Studio 14.0")
        SET(cmake_cxx_compiler_version "Microsoft Visual Studio 14.0")
    ENDIF()
ELSE()
    execute_process(COMMAND ${CMAKE_C_COMPILER} --version
            OUTPUT_VARIABLE cmake_c_compiler_version)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
            OUTPUT_VARIABLE cmake_cxx_compiler_version)
ENDIF(MSVC)
set(COMPILER_INFO "${CMAKE_C_COMPILER}:::${CMAKE_C_FLAGS_${GRCBTU}} ${CMAKE_C_FLAGS}\n${CMAKE_CXX_COMPILER}:::${CMAKE_CXX_FLAGS_${GRCBTU}} ${CMAKE_CXX_FLAGS}\n" )

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
set(GR_RUNTIME_DIR     bin CACHE PATH "Path to install all binaries")
set(GR_LIBRARY_DIR     lib${LIB_SUFFIX} CACHE PATH "Path to install libraries")
set(GR_INCLUDE_DIR     include CACHE PATH "Path to install header files")
set(GR_DATA_DIR        share CACHE PATH "Base location for data")
set(GR_PKG_DATA_DIR    ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package data")
set(GR_DOC_DIR         ${GR_DATA_DIR}/doc CACHE PATH "Path to install documentation")
set(GR_PKG_DOC_DIR     ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME}-${DOCVER} CACHE PATH "Path to install package docs")
set(GR_LIBEXEC_DIR     libexec CACHE PATH "Path to install libexec files")
set(GR_PKG_LIBEXEC_DIR ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME} CACHE PATH "Path to install package libexec files")
set(GRC_BLOCKS_DIR     ${GR_PKG_DATA_DIR}/grc/blocks CACHE PATH "Path to install GRC blocks")
set(GR_THEMES_DIR      ${GR_PKG_DATA_DIR}/themes CACHE PATH "Path to install QTGUI themes")

# Set location of config/prefs files in /etc
# Special exception if prefix is /usr so we don't make a /usr/etc.
set(GR_CONF_DIR etc CACHE PATH "Path to install config files")
string(COMPARE EQUAL "${CMAKE_INSTALL_PREFIX}" "/usr" isusr)
if(isusr)
  set(SYSCONFDIR "/${GR_CONF_DIR}" CACHE PATH "System configuration directory")
else(isusr)
  set(SYSCONFDIR "${CMAKE_INSTALL_PREFIX}/${GR_CONF_DIR}" CACHE PATH "System configuration directory" FORCE)
endif(isusr)

set(GR_PKG_CONF_DIR ${SYSCONFDIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install package configs")
set(GR_PREFSDIR     ${SYSCONFDIR}/${CMAKE_PROJECT_NAME}/conf.d CACHE PATH "Path to install preference files")

OPTION(ENABLE_PERFORMANCE_COUNTERS "Enable block performance counters" ON)
if(ENABLE_PERFORMANCE_COUNTERS)
  message(STATUS "ADDING PERF COUNTERS")
  SET(GR_PERFORMANCE_COUNTERS True)
  add_definitions(-DGR_PERFORMANCE_COUNTERS)
else(ENABLE_PERFORMANCE_COUNTERS)
  SET(GR_PERFORMANCE_COUNTERS False)
  message(STATUS "NO PERF COUNTERS")
endif(ENABLE_PERFORMANCE_COUNTERS)

OPTION(ENABLE_STATIC_LIBS "Enable building of static libraries" OFF)
message(STATUS "Building Static Libraries: ${ENABLE_STATIC_LIBS}")

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
        set(CMAKE_INSTALL_NAME_DIR
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install Name Destination Directory" FORCE)
    endif(NOT CMAKE_INSTALL_NAME_DIR)
    if(NOT CMAKE_INSTALL_RPATH)
        set(CMAKE_INSTALL_RPATH
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install RPath" FORCE)
    endif(NOT CMAKE_INSTALL_RPATH)
    if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE
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
find_package(PythonLibs 2)
find_package(SWIG)

if(SWIG_FOUND)
    # Minimum SWIG version required is 1.3.31
    set(SWIG_VERSION_CHECK FALSE)
    if("${SWIG_VERSION}" VERSION_GREATER "1.3.30")
        set(SWIG_VERSION_CHECK TRUE)
    endif()
endif(SWIG_FOUND)

include(GrComponent)
GR_REGISTER_COMPONENT("python-support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    SWIG_FOUND
    SWIG_VERSION_CHECK
)

find_package(CppUnit)
GR_REGISTER_COMPONENT("testing-support" ENABLE_TESTING
    CPPUNIT_FOUND
)

########################################################################
# Add optional dlls specified in DLL_PATHS
########################################################################
foreach(path ${DLL_PATHS})
    file(GLOB _dlls "${path}/*.dll")
    list(APPEND ALL_DLL_FILES ${_dlls})
endforeach(path)
if(DEFINED ALL_DLL_FILES)
    include(GrPackage)
    CPACK_COMPONENT("extra_dlls"
        DISPLAY_NAME "Extra DLLs"
        DESCRIPTION  "Extra DLLs for runtime dependency requirements"
    )
    message(STATUS "")
    message(STATUS "Including the following dlls into the install:")
    foreach(_dll ${ALL_DLL_FILES})
        message(STATUS "  ${_dll}")
    endforeach(_dll)
    install(FILES ${ALL_DLL_FILES} DESTINATION ${GR_RUNTIME_DIR} COMPONENT "extra_dlls")
endif()

########################################################################
# Setup volk as a subproject
########################################################################
message(STATUS "")
message(STATUS "Configuring VOLK support...")

OPTION(ENABLE_INTERNAL_VOLK "Enable internal VOLK only" ON)
unset(VOLK_FOUND)
if(NOT ENABLE_INTERNAL_VOLK)
    find_package(Volk)
    if(NOT VOLK_FOUND)
        message(STATUS "  External VOLK not found; checking internal.")
    endif()
endif()
if(NOT VOLK_FOUND)
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
        message(STATUS "    git submodule update")
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

    set(VOLK_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/volk/include
        ${CMAKE_CURRENT_BINARY_DIR}/volk/include
    )

    set(VOLK_LIBRARIES volk)

    set(VOLK_INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
    set(VOLK_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

    if(ENABLE_VOLK)

        include(GrPackage)
        CPACK_SET(CPACK_COMPONENT_GROUP_VOLK_DESCRIPTION "Vector optimized library of kernels")

        CPACK_COMPONENT("volk_runtime"
            GROUP        "Volk"
            DISPLAY_NAME "Runtime"
            DESCRIPTION  "Dynamic link libraries"
        )

        CPACK_COMPONENT("volk_devel"
            GROUP        "Volk"
            DISPLAY_NAME "Development"
            DESCRIPTION  "C++ headers, package config, import libraries"
        )

    endif(ENABLE_VOLK)
else()
    message(STATUS "  An external VOLK has been found and will be used for build.")
    set(ENABLE_VOLK TRUE)

    get_filename_component(VOLK_INSTALL_LIBRARY_DIR "${VOLK_LIBRARIES}" DIRECTORY)
    set(VOLK_INSTALL_INCLUDE_DIR ${VOLK_INCLUDE_DIRS})
endif(NOT VOLK_FOUND)

message(STATUS "  Override with -DENABLE_INTERNAL_VOLK=ON/OFF")

# Handle gr_log enable/disable
GR_LOGGING()

########################################################################
# Distribute the README file
########################################################################
install(
    FILES README README.hacking
    DESTINATION ${GR_PKG_DOC_DIR}
    COMPONENT "docs"
)

########################################################################
# The following dependency libraries are needed by all gr modules:
########################################################################
list(APPEND GR_TEST_TARGET_DEPS volk gnuradio-runtime)
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

########################################################################
# Add subdirectories (in order of deps)
########################################################################
add_subdirectory(docs)
add_subdirectory(gnuradio-runtime)
add_subdirectory(gr-blocks)
add_subdirectory(grc)
add_subdirectory(gr-fec)
add_subdirectory(gr-fft)
add_subdirectory(gr-filter)
add_subdirectory(gr-analog)
add_subdirectory(gr-digital)
add_subdirectory(gr-dtv)
add_subdirectory(gr-atsc)
add_subdirectory(gr-audio)
add_subdirectory(gr-comedi)
add_subdirectory(gr-channels)
add_subdirectory(gr-noaa)
add_subdirectory(gr-pager)
add_subdirectory(gr-qtgui)
add_subdirectory(gr-trellis)
add_subdirectory(gr-uhd)
add_subdirectory(gr-utils)
add_subdirectory(gr-video-sdl)
add_subdirectory(gr-vocoder)
add_subdirectory(gr-fcd)
add_subdirectory(gr-wavelet)
add_subdirectory(gr-wxgui)
add_subdirectory(gr-zeromq)

# Defining GR_CTRLPORT for gnuradio/config.h
if(ENABLE_GR_CTRLPORT)
  set(GR_CTRLPORT True)
  add_definitions(-DGR_CTRLPORT)

  if(CTRLPORT_BACKENDS GREATER 0)
    set(GR_RPCSERVER_ENABLED True)

    if(THRIFT_FOUND)
      set(GR_RPCSERVER_THRIFT True)
    endif(THRIFT_FOUND)
  endif(CTRLPORT_BACKENDS GREATER 0)
endif(ENABLE_GR_CTRLPORT)

# Install our Cmake modules into $prefix/lib/cmake/gnuradio
# See "Package Configuration Files" on page:
#    http://www.cmake.org/Wiki/CMake/Tutorials/Packaging

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/Modules/GnuradioConfig.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
@ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/Modules/GnuradioConfigVersion.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake
@ONLY)

SET(cmake_configs
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfig.cmake
  ${CMAKE_BINARY_DIR}/cmake/Modules/GnuradioConfigVersion.cmake
)

if(NOT CMAKE_MODULES_DIR)
  set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

# Install all other cmake files into same directory
file(GLOB cmake_others "cmake/Modules/*.cmake")
list(REMOVE_ITEM cmake_others
    "${CMAKE_SOURCE_DIR}/cmake/Modules/FindGnuradio.cmake"
)

install(
  FILES ${cmake_configs} ${cmake_others}
  DESTINATION ${CMAKE_MODULES_DIR}/gnuradio
  COMPONENT "runtime_devel"
)

#finalize cpack after subdirs processed
include(GrPackage)
CPACK_FINALIZE()

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
    COMPONENT "runtime_devel"
)
