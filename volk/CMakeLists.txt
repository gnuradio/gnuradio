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
cmake_minimum_required(VERSION 2.6)
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose build type: None Debug Release RelWithDebInfo MinSizeRel")
project(volk)
enable_language(CXX)
enable_language(C)
enable_testing()
set(VERSION 0.1)
set(LIBVER 0.0.0)

set(CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}) #allows this to be a sub-project
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}) #allows this to be a sub-project
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) #location for custom "Modules"

########################################################################
# Dependencies setup
########################################################################
find_package(PythonInterp REQUIRED PYTHON_EXECUTABLE)
find_package(Boost COMPONENTS unit_test_framework)
find_package(ORC)


########################################################################
# Setup the package config file
########################################################################
#set variables found in the pc.in file
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
set(includedir "\${prefix}/include")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/volk.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/volk.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/volk.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
    COMPONENT "volk_devel"
)

########################################################################
# Install all headers in the include directories
########################################################################
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/volk
    DESTINATION include COMPONENT "volk_devel"
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    ${CMAKE_BINARY_DIR}/include/volk/volk.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_cpu.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_config_fixed.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_typedefs.h
    DESTINATION include/volk
    COMPONENT "volk_devel"
)

########################################################################
# Setup the library
########################################################################
add_subdirectory(lib)

########################################################################
# And the utility apps
########################################################################
add_subdirectory(apps)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
