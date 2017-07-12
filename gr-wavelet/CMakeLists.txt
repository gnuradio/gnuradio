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

find_package(GSL)

########################################################################
# Register component
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("gr-wavelet" ENABLE_GR_WAVELET
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
    ENABLE_GR_ANALOG
    GSL_FOUND
)

GR_SET_GLOBAL(GR_WAVELET_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_WAVELET)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_WAVELET_DESCRIPTION "GNU Radio Wavelet Blocks")

CPACK_COMPONENT("wavelet_runtime"
    GROUP        "WAVELET"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("wavelet_devel"
    GROUP        "WAVELET"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("wavelet_python"
    GROUP        "WAVELET"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;wavelet_runtime"
)

CPACK_COMPONENT("wavelet_swig"
    GROUP        "WAVELET"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;wavelet_python;wavelet_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/wavelet)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/wavelet)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-wavelet.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "wavelet_devel"
)

endif(ENABLE_GR_WAVELET)
