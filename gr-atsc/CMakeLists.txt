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
GR_REGISTER_COMPONENT("gr-atsc" ENABLE_GR_ATSC
    Boost_FOUND
    ENABLE_GR_CORE
)

GR_SET_GLOBAL(GR_ATSC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_ATSC)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_ATSC_DESCRIPTION "GNU Radio ATSC Blocks")

CPACK_COMPONENT("atsc_runtime"
    GROUP        "ATSC"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("atsc_devel"
    GROUP        "ATSC"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("atsc_python"
    GROUP        "ATSC"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;atsc_runtime"
)

CPACK_COMPONENT("atsc_examples"
    GROUP        "ATSC"
    DISPLAY_NAME "Examples"
    DESCRIPTION  "Python examples for ATSC"
    DEPENDS      "atsc_python"
)

CPACK_COMPONENT("atsc_swig"
    GROUP        "ATSC"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;atsc_python;atsc_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src/lib)
if(ENABLE_PYTHON)
    add_subdirectory(src/python)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-atsc.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-atsc.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-atsc.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "atsc_devel"
)

endif(ENABLE_GR_ATSC)
