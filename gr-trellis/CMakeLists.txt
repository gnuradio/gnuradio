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
GR_REGISTER_COMPONENT("gr-trellis" ENABLE_GR_TRELLIS
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_ANALOG
    ENABLE_GR_BLOCKS
    ENABLE_GR_DIGITAL
)

GR_SET_GLOBAL(GR_TRELLIS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

GR_SET_GLOBAL(GR_TRELLIS_SWIG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/swig
)

SET(GR_PKG_TRELLIS_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/trellis)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_TRELLIS)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_TRELLIS_DESCRIPTION "GNU Radio Trellis Blocks")

CPACK_COMPONENT("trellis_docs"
    GROUP        "Trellis"
    DISPLAY_NAME "Documentation"
    DESCRIPTION  "Doxygen HTML and XML"
)

CPACK_COMPONENT("trellis_runtime"
    GROUP        "Trellis"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("trellis_devel"
    GROUP        "Trellis"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("trellis_python"
    GROUP        "Trellis"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "runtime_python;trellis_runtime"
)

CPACK_COMPONENT("trellis_examples"
    GROUP        "Trellis"
    DISPLAY_NAME "Examples"
    DESCRIPTION  "Python examples for trellis"
    DEPENDS      "trellis_python"
)

CPACK_COMPONENT("trellis_swig"
    GROUP        "Trellis"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;trellis_python;trellis_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/trellis)
add_subdirectory(lib)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/trellis)
    add_subdirectory(examples/python)
    add_subdirectory(examples/grc)
    add_subdirectory(grc)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-trellis.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "trellis_devel"
)

endif(ENABLE_GR_TRELLIS)
