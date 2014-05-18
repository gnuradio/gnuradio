# Copyright 2014 Free Software Foundation, Inc.
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

GR_REGISTER_COMPONENT("gr-dtv" ENABLE_GR_DTV
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
)

GR_SET_GLOBAL(GR_DTV_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/lib
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_DTV)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_DTV_DESCRIPTION "GNU Radio DTV Blocks")

CPACK_COMPONENT("dtv_runtime"
    GROUP        "DTV"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("dtv_devel"
    GROUP        "DTV"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("dtv_python"
    GROUP        "DTV"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;dtv_runtime"
)

CPACK_COMPONENT("dtv_swig"
    GROUP        "DTV"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;dtv_python;dtv_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/dtv)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/dtv)
    add_subdirectory(grc)
    add_subdirectory(examples)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
add_subdirectory(doc)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-dtv.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "dtv_devel"
)

endif(ENABLE_GR_DTV)
