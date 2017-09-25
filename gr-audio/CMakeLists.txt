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
GR_REGISTER_COMPONENT("gr-audio" ENABLE_GR_AUDIO
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
)

GR_SET_GLOBAL(GR_AUDIO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(GR_PKG_AUDIO_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/audio)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_AUDIO)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_AUDIO_DESCRIPTION "GNU Radio Audio Blocks")

CPACK_COMPONENT("audio_runtime"
    GROUP        "Audio"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("audio_devel"
    GROUP        "Audio"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("audio_python"
    GROUP        "Audio"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;audio_runtime"
)

CPACK_COMPONENT("audio_examples"
    GROUP        "Audio"
    DISPLAY_NAME "Examples"
    DESCRIPTION  "Example programs"
    DEPENDS      "audio_runtime"
)

CPACK_COMPONENT("audio_swig"
    GROUP        "Audio"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;audio_python;audio_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(lib)
add_subdirectory(include/gnuradio/audio)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/audio)
    add_subdirectory(grc)
    add_subdirectory(examples/python)
    add_subdirectory(examples/grc)
endif(ENABLE_PYTHON)

if(ENABLE_GR_ANALOG)
  add_subdirectory(examples/c++)
endif(ENABLE_GR_ANALOG)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-audio.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "audio_devel"
)

endif(ENABLE_GR_AUDIO)
