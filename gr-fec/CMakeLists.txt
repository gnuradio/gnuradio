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

find_package(GSL)

GR_REGISTER_COMPONENT("gr-fec" ENABLE_GR_FEC
    ENABLE_VOLK
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
)

GR_SET_GLOBAL(GR_FEC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/viterbi
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reed-solomon
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(GR_PKG_FEC_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/fec)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FEC)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_FEC_DESCRIPTION "GNU Radio FEC Blocks")

CPACK_COMPONENT("fec_runtime"
    GROUP        "FEC"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("fec_devel"
    GROUP        "FEC"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("fec_python"
    GROUP        "FEC"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;fec_runtime"
)

CPACK_COMPONENT("fec_swig"
    GROUP        "FEC"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;fec_python;fec_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/fec)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/fec)
    add_subdirectory(python/fec/LDPC)
    add_subdirectory(grc)
endif(ENABLE_PYTHON)
add_subdirectory(examples)
add_subdirectory(docs)
add_subdirectory(ldpc_alist)
add_subdirectory(apps)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fec.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "fec_devel"
)

endif(ENABLE_GR_FEC)
