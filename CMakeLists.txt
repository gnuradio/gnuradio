# Copyright 2010-2011 Free Software Foundation, Inc.
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
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gnuradio CXX C)
ENABLE_TESTING()

#select the release build type by default to get optimization flags
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE "Release")
   MESSAGE(STATUS "Build type not specified: defaulting to release.")
ENDIF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

INCLUDE(GrVersion) #setup version info

########################################################################
# Import executables from a native build (for cross compiling)
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
IF(IMPORT_EXECUTABLES)
    INCLUDE(${IMPORT_EXECUTABLES})
ENDIF(IMPORT_EXECUTABLES)

#set file that the native build will fill with exports
SET(EXPORT_FILE ${CMAKE_BINARY_DIR}/ImportExecutables.cmake)
FILE(WRITE ${EXPORT_FILE}) #blank the file (subdirs will append)

########################################################################
# Compiler specific setup
########################################################################
IF(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    ADD_DEFINITIONS(-fvisibility=hidden)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-Wsign-compare)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(MSVC)
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cmake/msvc) #missing headers
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    ADD_DEFINITIONS(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    ADD_DEFINITIONS( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    ADD_DEFINITIONS(-DHAVE_CONFIG_H)
    ADD_DEFINITIONS(/MP) #build with multiple processors
ENDIF(MSVC)

########################################################################
# Install directories
########################################################################
INCLUDE(GrPlatform) #define LIB_SUFFIX
SET(GR_RUNTIME_DIR      bin)
SET(GR_LIBRARY_DIR      lib${LIB_SUFFIX})
SET(GR_INCLUDE_DIR      include)
SET(GR_DATA_DIR         share)
SET(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
SET(GR_DOC_DIR          ${GR_DATA_DIR}/doc)
SET(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME}-${LIBVER})
SET(GR_CONF_DIR         etc)
SET(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME})
SET(GR_LIBEXEC_DIR      libexec)
SET(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})
SET(GRC_BLOCKS_DIR      ${GR_PKG_DATA_DIR}/grc/blocks)

########################################################################
# Variables replaced when configuring the package config files
########################################################################
FILE(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}"           prefix)
FILE(TO_NATIVE_PATH "\${prefix}"                        exec_prefix)
FILE(TO_NATIVE_PATH "\${exec_prefix}/${GR_LIBRARY_DIR}" libdir)
FILE(TO_NATIVE_PATH "\${prefix}/${GR_INCLUDE_DIR}"      includedir)

########################################################################
# Create uninstall target
########################################################################
CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

ADD_CUSTOM_TARGET(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Enable python component
########################################################################
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(SWIG)
INCLUDE(GrComponent)
GR_REGISTER_COMPONENT("python-support" ENABLE_PYTHON
    PYTHONLIBS_FOUND
    SWIG_FOUND
)

FIND_PACKAGE(CppUnit)
GR_REGISTER_COMPONENT("testing-support" ENABLE_TESTING
    CPPUNIT_FOUND
)

########################################################################
# Add optional dlls specified in DLL_PATHS
########################################################################
FOREACH(path ${DLL_PATHS})
    FILE(GLOB _dlls "${path}/*.dll")
    LIST(APPEND ALL_DLL_FILES ${_dlls})
ENDFOREACH(path)
IF(DEFINED ALL_DLL_FILES)
    INCLUDE(GrPackage)
    CPACK_COMPONENT("extra_dlls"
        DISPLAY_NAME "Extra DLLs"
        DESCRIPTION  "Extra DLLs for runtime dependency requirements"
    )
    MESSAGE(STATUS "")
    MESSAGE(STATUS "Including the following dlls into the install:")
    FOREACH(_dll ${ALL_DLL_FILES})
        MESSAGE(STATUS "  ${_dll}")
    ENDFOREACH(_dll)
    INSTALL(FILES ${ALL_DLL_FILES} DESTINATION ${GR_RUNTIME_DIR} COMPONENT "extra_dlls")
ENDIF()

########################################################################
# Setup volk as a subproject
########################################################################
INCLUDE(GrComponent)
GR_REGISTER_COMPONENT("volk" ENABLE_VOLK)

SET(VOLK_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/volk/include
    ${CMAKE_CURRENT_BINARY_DIR}/volk/include
)

IF(ENABLE_VOLK)

INCLUDE(GrPackage)
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


ADD_SUBDIRECTORY(volk)
ENDIF(ENABLE_VOLK)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(gruel)
ADD_SUBDIRECTORY(gnuradio-core)
ADD_SUBDIRECTORY(grc)
ADD_SUBDIRECTORY(docs)

ADD_SUBDIRECTORY(gr-atsc)
ADD_SUBDIRECTORY(gr-audio)
ADD_SUBDIRECTORY(gr-comedi)
ADD_SUBDIRECTORY(gr-digital)
ADD_SUBDIRECTORY(gr-noaa)
ADD_SUBDIRECTORY(gr-pager)
ADD_SUBDIRECTORY(gr-qtgui)
ADD_SUBDIRECTORY(gr-trellis)
ADD_SUBDIRECTORY(gr-uhd)
ADD_SUBDIRECTORY(gr-video-sdl)
ADD_SUBDIRECTORY(gr-vocoder)
ADD_SUBDIRECTORY(gr-wxgui)

#finalize cpack after subdirs processed
INCLUDE(GrPackage)
CPACK_FINALIZE()

########################################################################
# Print summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Building for version: ${VERSION}")
