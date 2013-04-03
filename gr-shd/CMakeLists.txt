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
# Setup dependencies
########################################################################
include(GrBoost)

PKG_CHECK_MODULES(PC_SHD shd)
FIND_PATH(
    SHD_INCLUDE_DIRS
    NAMES shd/config.hpp
    HINTS $ENV{SHD_DIR}/include
        ${PC_SHD_INCLUDE_DIRS}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SHD_LIBRARIES
    NAMES shd
    HINTS $ENV{SHD_DIR}/lib
        ${PC_SHD_LIBRARIES}
    PATHS /usr/local/lib
          /usr/lib
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SHD DEFAULT_MSG SHD_LIBRARIES SHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(SHD_LIBRARIES SHD_INCLUDE_DIRS)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-shd" ENABLE_GR_SHD
    Boost_FOUND
    SHD_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_ANALOG
)

GR_SET_GLOBAL(GR_SHD_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_SHD)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_SHD_DESCRIPTION "GNU Radio SHD Blocks")

CPACK_COMPONENT("shd_runtime"
    GROUP        "SHD"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("shd_devel"
    GROUP        "SHD"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("shd_python"
    GROUP        "SHD"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;shd_runtime"
)

CPACK_COMPONENT("shd_swig"
    GROUP        "SHD"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;shd_python;shd_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-shd.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-shd.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-shd.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "shd_devel"
)

endif(ENABLE_GR_SHD)
