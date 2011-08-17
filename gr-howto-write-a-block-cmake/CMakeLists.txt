# Copyright 2011 Free Software Foundation, Inc.
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
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gr-howto-write-a-block CXX)
ENABLE_TESTING()

#select the release build type by default to get optimization flags
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE "Release")
   MESSAGE(STATUS "Build type not specified: defaulting to release.")
ENDIF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

########################################################################
# Compiler specific setup
########################################################################
IF(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    ADD_DEFINITIONS(-fvisibility=hidden)
ENDIF()

########################################################################
# Find boost
########################################################################
IF(UNIX AND EXISTS "/usr/lib64")
    LIST(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
ENDIF(UNIX AND EXISTS "/usr/lib64")
SET(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
FIND_PACKAGE(Boost "1.35")

IF(NOT Boost_FOUND)
    MESSAGE(FATAL_ERROR "Boost required to compile howto")
ENDIF()

########################################################################
# Find gnuradio build dependencies
########################################################################
FIND_PACKAGE(Gruel)
FIND_PACKAGE(GnuradioCore)

IF(NOT GRUEL_FOUND)
    MESSAGE(FATAL_ERROR "Gruel required to compile howto")
ENDIF()

IF(NOT GNURADIO_CORE_FOUND)
    MESSAGE(FATAL_ERROR "GnuRadio Core required to compile howto")
ENDIF()

########################################################################
# Setup the include and linker paths
########################################################################
INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/lib
    ${Boost_INCLUDE_DIRS}
    ${GRUEL_INCLUDE_DIRS}
    ${GNURADIO_CORE_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
    ${Boost_LIBRARY_DIRS}
    ${GRUEL_LIBRARY_DIRS}
    ${GNURADIO_CORE_LIBRARY_DIRS}
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(swig)
ADD_SUBDIRECTORY(python)
ADD_SUBDIRECTORY(grc)
ADD_SUBDIRECTORY(apps)
