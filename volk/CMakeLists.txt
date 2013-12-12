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
# Environment setup
########################################################################
IF(NOT DEFINED BOOST_ROOT)
    SET(BOOST_ROOT ${CMAKE_INSTALL_PREFIX})
ENDIF()

IF(NOT DEFINED CROSSCOMPILE_MULTILIB)
    SET(CROSSCOMPILE_MULTILIB "")
ENDIF()
SET(CROSSCOMPILE_MULTILIB ${CROSSCOMPILE_MULTILIB} CACHE STRING "Define \"true\" if you have and want to use multiple C development libs installed for cross compile")


########################################################################
# Dependencies setup
########################################################################
include(GrPython) #sets PYTHON_EXECUTABLE and PYTHON_DASH_B
GR_PYTHON_CHECK_MODULE("python >= 2.5"     sys          "sys.version.split()[0] >= '2.5'"           PYTHON_MIN_VER_FOUND)
GR_PYTHON_CHECK_MODULE("Cheetah >= 2.0.0"  Cheetah      "Cheetah.Version >= '2.0.0'"                CHEETAH_FOUND)

if(NOT PYTHON_MIN_VER_FOUND)
    message(FATAL_ERROR "Python 2.5 or greater required to build VOLK")
endif()

if(NOT CHEETAH_FOUND)
    message(FATAL_ERROR "Cheetah templates required to build VOLK")
endif()

if(MSVC)
    if (NOT DEFINED BOOST_ALL_DYN_LINK)
        set(BOOST_ALL_DYN_LINK TRUE)
    endif()
    set(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    if(BOOST_ALL_DYN_LINK)
        add_definitions(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    else(BOOST_ALL_DYN_LINK)
        unset(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    endif(BOOST_ALL_DYN_LINK)
endif(MSVC)
include(GrBoost)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "VOLK Requires boost to build")
endif()

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
    DIRECTORY ${CMAKE_SOURCE_DIR}/kernels/volk
    DESTINATION include COMPONENT "volk_devel"
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    ${CMAKE_SOURCE_DIR}/include/volk/volk_prefs.h
    ${CMAKE_SOURCE_DIR}/include/volk/volk_complex.h
    ${CMAKE_SOURCE_DIR}/include/volk/volk_common.h
    ${CMAKE_BINARY_DIR}/include/volk/volk.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_cpu.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_config_fixed.h
    ${CMAKE_BINARY_DIR}/include/volk/volk_typedefs.h
    DESTINATION include/volk
    COMPONENT "volk_devel"
)

########################################################################
# Install cmake search routine for external use
########################################################################
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VolkConfig.cmake
    DESTINATION lib${LIB_SUFFIX}/cmake/volk/
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
add_subdirectory(python/volk_modtool)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
