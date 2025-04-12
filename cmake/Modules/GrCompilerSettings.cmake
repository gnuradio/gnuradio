# Copyright 2011-2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_COMPILER_SETTINGS_CMAKE)
    return()
endif()
set(__INCLUDED_GR_COMPILER_SETTING_CMAKE TRUE)

########################################################################
# Compiler version setup
########################################################################
# Append -O2 optimization flag for Debug builds (Not on MSVC since conflicts with RTC1 flag)
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O2")
endif()

# Check compiler version against our minimum
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(DEFINED CMAKE_CXX_COMPILER_VERSION)
        if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS ${GCC_MIN_VERSION})
            message(
                WARNING
                    "\nThe compiler selected to build GNU Radio (GCC version ${CMAKE_CXX_COMPILER_VERSION} : ${CMAKE_CXX_COMPILER}) is older than that officially supported (${GCC_MIN_VERSION} minimum). This build may or not work. We highly recommend using a more recent GCC version."
            )
        endif()
    else()
        message(
            WARNING
                "\nCannot determine the version of the compiler selected to build GNU Radio (GCC : ${CMAKE_CXX_COMPILER}). This build may or not work. We highly recommend using GCC version ${GCC_MIN_VERSION} or more recent."
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -v
        RESULT_VARIABLE _res
        ERROR_VARIABLE _err
        ERROR_STRIP_TRAILING_WHITESPACE)
    if(${_res} STREQUAL "0")
        # output is in error stream
        string(REGEX MATCH "^Apple.*" IS_APPLE ${_err})
        if("${IS_APPLE}" STREQUAL "")
            set(MIN_VERSION ${CLANG_MIN_VERSION})
            set(APPLE_STR "")
            # retrieve the compiler's version from it
            string(REGEX MATCH "clang version [0-9.]+" CLANG_OTHER_VERSION ${_err})
            string(REGEX MATCH "[0-9.]+" CLANG_VERSION ${CLANG_OTHER_VERSION})
        else()
            set(MIN_VERSION ${APPLECLANG_MIN_VERSION})
            set(APPLE_STR "Apple ")
            # retrieve the compiler's version from it
            string(REGEX MATCH "(clang-[0-9.]+)" CLANG_APPLE_VERSION ${_err})
            string(REGEX MATCH "[0-9.]+" CLANG_VERSION ${CLANG_APPLE_VERSION})
        endif()
        if(${CLANG_VERSION} VERSION_LESS "${MIN_VERSION}")
            message(
                WARNING
                    "\nThe compiler selected to build GNU Radio (${APPLE_STR}Clang version ${CLANG_VERSION} : ${CMAKE_CXX_COMPILER}) is older than that officially supported (${MIN_VERSION} minimum). This build may or not work. We highly recommend using Apple Clang version ${APPLECLANG_MIN_VERSION} or more recent, or Clang version ${CLANG_MIN_VERSION} or more recent."
            )
        endif()
    else()
        message(
            WARNING
                "\nCannot determine the version of the compiler selected to build GNU Radio (${APPLE_STR}Clang : ${CMAKE_CXX_COMPILER}). This build may or not work. We highly recommend using Apple Clang version ${APPLECLANG_MIN_VERSION} or more recent, or Clang version ${CLANG_MIN_VERSION} or more recent."
        )
    endif()
else()
    message(STATUS "Skipping compiler version check.")
endif()

# Configure C++ standard if not externally specified (will actually be
# set after CppUnit check below). Use the variable CMAKE_CXX_STANDARD
# since it will actually be used for this purposes starting in CMake 3.1.

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(GR_CXX_VERSION_FEATURE cxx_std_${CMAKE_CXX_STANDARD})

########################################################################
# Compiler specific setup
########################################################################
include(GrMiscUtils) #compiler flag check

include(TestBigEndian)
test_big_endian(GR_IS_BIG_ENDIAN)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT WIN32)
        #http://gcc.gnu.org/wiki/Visibility
        gr_add_cxx_compiler_flag_if_available(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
    endif(NOT WIN32)
    gr_add_cxx_compiler_flag_if_available(-Wsign-compare HAVE_WARN_SIGN_COMPARE)
    gr_add_cxx_compiler_flag_if_available(-Wall HAVE_WARN_ALL)
    gr_add_cxx_compiler_flag_if_available(-Wno-uninitialized HAVE_WARN_NO_UNINITIALIZED)
    gr_add_cxx_compiler_flag_if_available(-Wignored-qualifiers HAVE_WARN_IGNORED_QUALIFIERS)
    gr_add_cxx_compiler_flag_if_available(-Wcast-qual HAVE_WARN_CAST_QUAL)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

if(MSVC)
    include_directories(${PROJECT_SOURCE_DIR}/cmake/msvc) #missing headers
    add_compile_options(/MP) #build with multiple processors
    add_compile_options(/bigobj) #allow for larger object files
endif(MSVC)
