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

set(GR_CXX_VERSION_FEATURE cxx_std_${CMAKE_CXX_STANDARD})

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
    include_directories(${PROJECT_SOURCE_DIR}/cmake/msvc) #missing headers
    add_compile_options(/MP) #build with multiple processors
    add_compile_options(/bigobj) #allow for larger object files
endif(MSVC)
