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

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-vocoder" ENABLE_GR_VOCODER
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_BLOCKS
    ENABLE_GR_FILTER
    ENABLE_GR_ANALOG
)

GR_SET_GLOBAL(GR_VOCODER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

SET(GR_PKG_VOCODER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/vocoder)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_VOCODER)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_VOCODER_DESCRIPTION "GNU Radio Vocoder Blocks")

CPACK_COMPONENT("vocoder_runtime"
    GROUP        "Vocoder"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("vocoder_devel"
    GROUP        "Vocoder"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("vocoder_python"
    GROUP        "Vocoder"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "runtime_python;vocoder_runtime"
)

CPACK_COMPONENT("vocoder_examples"
    GROUP        "Vocoder"
    DISPLAY_NAME "Examples"
    DESCRIPTION  "Python examples for vocoder"
    DEPENDS      "vocoder_python"
)

CPACK_COMPONENT("vocoder_swig"
    GROUP        "Vocoder"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;vocoder_python;vocoder_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/vocoder)
add_subdirectory(lib)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/vocoder)
    add_subdirectory(grc)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-vocoder.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "vocoder_devel"
)

endif(ENABLE_GR_VOCODER)
