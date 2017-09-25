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
GR_REGISTER_COMPONENT("gr-analog" ENABLE_GR_ANALOG
    Boost_FOUND
    ENABLE_VOLK
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
)

GR_SET_GLOBAL(GR_ANALOG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

GR_SET_GLOBAL(GR_ANALOG_SWIG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/swig
)

SET(GR_PKG_ANALOG_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/analog)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_ANALOG)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_ANALOG_DESCRIPTION "GNU Radio Analog Blocks")

CPACK_COMPONENT("analog_runtime"
    GROUP        "Analog"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("analog_devel"
    GROUP        "Analog"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("analog_python"
    GROUP        "Analog"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "runtime_python;analog_runtime"
)

CPACK_COMPONENT("analog_swig"
    GROUP        "Analog"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;analog_python;analog_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/analog)
add_subdirectory(lib)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/analog)
    add_subdirectory(grc)
    add_subdirectory(examples)
    add_subdirectory(examples/tags)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-analog.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "analog_devel"
)

endif(ENABLE_GR_ANALOG)
