# Copyright 2011,2012 Free Software Foundation, Inc.
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

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-noaa" ENABLE_GR_NOAA
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
)

GR_SET_GLOBAL(GR_NOAA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

GR_SET_GLOBAL(GR_NOAA_SWIG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/swig
)

SET(GR_PKG_NOAA_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/noaa)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_NOAA)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_NOAA_DESCRIPTION "GNU Radio NOAA Blocks")

CPACK_COMPONENT("noaa_runtime"
    GROUP        "NOAA"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("noaa_devel"
    GROUP        "NOAA"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("noaa_python"
    GROUP        "NOAA"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;noaa_runtime"
)

CPACK_COMPONENT("noaa_swig"
    GROUP        "NOAA"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;noaa_python;noaa_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/noaa)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/noaa)
    add_subdirectory(grc)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-noaa.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-noaa.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-noaa.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "noaa_devel"
)

endif(ENABLE_GR_NOAA)
