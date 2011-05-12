#
# Copyright 2011 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose build type: None Debug Release RelWithDebInfo MinSizeRel")
ELSE()
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type: None Debug Release RelWithDebInfo MinSizeRel")
ENDIF()
PROJECT(volk)
ENABLE_LANGUAGE(CXX)
ENABLE_LANGUAGE(C)
ENABLE_TESTING()
SET(VERSION 0.1)
SET(LIBVER 0.0.0)

########################################################################
# Dependencies setup
########################################################################
FIND_PACKAGE(PythonInterp)
IF(NOT PYTHONINTERP_FOUND)
    MESSAGE(FATAL_ERROR "Python interpreter required by the build system.")
ENDIF(NOT PYTHONINTERP_FOUND)

########################################################################
# Create rules to run the volk generator
########################################################################
#only list some of the generated sources (enough to make the rules work)
SET(some_gen_sources
    ${CMAKE_BINARY_DIR}/lib/volk.c
    ${CMAKE_BINARY_DIR}/include/volk/volk.h
)

#dependencies are all python and xml files
FILE(GLOB xml_files ${CMAKE_SOURCE_DIR}/gen/*.xml)
FILE(GLOB py_files ${CMAKE_SOURCE_DIR}/gen/*.py)

ADD_CUSTOM_COMMAND(
    OUTPUT ${some_gen_sources}
    DEPENDS ${xml_files} ${py_files}
    COMMAND ${PYTHON_EXECUTABLE} -B
        ${CMAKE_SOURCE_DIR}/gen/volk_register.py
        ${CMAKE_BINARY_DIR}
    COMMENT "Generating volk sources..."
)
ADD_CUSTOM_TARGET(volk_register DEPENDS ${some_gen_sources})

########################################################################
# Setup the package config file
########################################################################
#set variables found in the pc.in file
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix "\${prefix}")
SET(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
SET(includedir "\${prefix}/include")

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/volk.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/volk.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/volk.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
)

########################################################################
# Install all headers in the include directories
########################################################################
INSTALL(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/volk
    DIRECTORY ${CMAKE_BINARY_DIR}/include/volk
    DESTINATION include FILES_MATCHING PATTERN "*.h"
)

########################################################################
# Setup the library
########################################################################
ADD_SUBDIRECTORY(lib)

########################################################################
# And the utility apps
########################################################################
ADD_SUBDIRECTORY(apps)

########################################################################
# Print summary
########################################################################
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
