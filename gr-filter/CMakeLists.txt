# Copyright 2012 Free Software Foundation, Inc.
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

GR_REGISTER_COMPONENT("gr-filter" ENABLE_GR_FILTER
    ENABLE_VOLK
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_BLOCKS
)

GR_SET_GLOBAL(GR_FILTER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${GR_FFT_INCLUDE_DIRS}
)

SET(GR_PKG_FILTER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/filter)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FILTER)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_FILTER_DESCRIPTION "GNU Radio Filter Blocks")

CPACK_COMPONENT("filter_runtime"
    GROUP        "Filter"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("filter_devel"
    GROUP        "Filter"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("filter_python"
    GROUP        "Filter"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;filter_runtime"
)

CPACK_COMPONENT("filter_swig"
    GROUP        "Filter"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;filter_python;filter_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/filter)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/filter)
    add_subdirectory(python/filter/design)
    add_subdirectory(python/filter/gui)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
add_subdirectory(examples)
add_subdirectory(docs)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "filter_devel"
)

endif(ENABLE_GR_FILTER)
