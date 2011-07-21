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
INCLUDE(GrBoost)

########################################################################
# Register component
########################################################################
INCLUDE(GrComponent)
GR_REGISTER_COMPONENT("gr-audio" ENABLE_GR_AUDIO
    Boost_FOUND
    ENABLE_GR_CORE_
)

GR_SET_GLOBAL(GR_AUDIO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_AUDIO)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_AUDIO_DESCRIPTION "The GNU Radio Audio Blocks")

CPACK_COMPONENT("audio_runtime"
    GROUP        "Audio"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("audio_devel"
    GROUP        "Audio"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("audio_python"
    GROUP        "Audio"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;audio_runtime"
)

CPACK_COMPONENT("audio_swig"
    GROUP        "Audio"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;audio_python;audio_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(examples/c++)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(grc)
    ADD_SUBDIRECTORY(examples/python)
ENDIF(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-audio.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "audio_devel"
)

ENDIF(ENABLE_GR_AUDIO)
