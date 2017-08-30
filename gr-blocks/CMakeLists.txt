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
GR_REGISTER_COMPONENT("gr-blocks" ENABLE_GR_BLOCKS
    Boost_FOUND
    ENABLE_VOLK
    ENABLE_GNURADIO_RUNTIME
)

GR_SET_GLOBAL(GR_BLOCKS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${VOLK_INCLUDE_DIRS}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_BLOCKS)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_BLOCKS_DESCRIPTION "GNU Radio Basic Blocks")

CPACK_COMPONENT("blocks_runtime"
    GROUP        "Blocks"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("blocks_devel"
    GROUP        "Blocks"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("blocks_python"
    GROUP        "Blocks"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;blocks_runtime"
)

CPACK_COMPONENT("blocks_swig"
    GROUP        "Blocks"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;blocks_python;blocks_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/blocks)
add_subdirectory(lib)
#if(ENABLE_TESTING)
#  add_subdirectory(tests)
#endif(ENABLE_TESTING)
if(ENABLE_PYTHON)
     add_subdirectory(python/blocks)
     add_subdirectory(python/grc_gnuradio)
     add_subdirectory(swig)
     add_subdirectory(grc)
     add_subdirectory(docs)
     add_subdirectory(examples)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-blocks.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "blocks_devel"
)

endif(ENABLE_GR_BLOCKS)
