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
if(NOT WIN32 AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include(FindUSB)
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)

if (CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME MATCHES kFreeBSD OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    GR_REGISTER_COMPONENT("gr-fcd" ENABLE_GR_FCD
        Boost_FOUND
        ENABLE_GNURADIO_RUNTIME
        ENABLE_GR_BLOCKS
        ENABLE_GR_AUDIO
        LIBUSB_FOUND
    )
else()
    GR_REGISTER_COMPONENT("gr-fcd" ENABLE_GR_FCD
        Boost_FOUND
        ENABLE_GNURADIO_RUNTIME
        ENABLE_GR_AUDIO
    )
endif()

GR_SET_GLOBAL(GR_FCD_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(GR_PKG_FCD_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/fcd)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FCD)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_FCD_DESCRIPTION "GNU Radio FCD Blocks")

CPACK_COMPONENT("fcd_runtime"
    GROUP        "FCD"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime;audio_runtime"
)

CPACK_COMPONENT("fcd_devel"
    GROUP        "FCD"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel;audio_devel"
)

CPACK_COMPONENT("fcd_python"
    GROUP        "FCD"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;fcd_runtime"
)

CPACK_COMPONENT("fcd_examples"
    GROUP        "FCD"
    DISPLAY_NAME "Examples"
    DESCRIPTION  "Example programs"
    DEPENDS      "fcd_runtime"
)

CPACK_COMPONENT("fcd_swig"
    GROUP        "FCD"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;fcd_python;fcd_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/fcd)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/fcd)
    add_subdirectory(grc)
    add_subdirectory(examples/grc)
endif(ENABLE_PYTHON)
add_subdirectory(docs)

if(ENABLE_GR_ANALOG)
  add_subdirectory(examples/c++)
endif(ENABLE_GR_ANALOG)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fcd.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fcd.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fcd.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "fcd_devel"
)

endif(ENABLE_GR_FCD)
