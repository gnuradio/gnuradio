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
GR_REGISTER_COMPONENT("gr-vocoder" ENABLE_GR_VOCODER
    Boost_FOUND
    ENABLE_GR_CORE_
)

GR_SET_GLOBAL(GR_VOCODER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_VOCODER)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_vocoder_DESCRIPTION "The GNU Radio Vocoder Blocks")

CPACK_COMPONENT("vocoder_runtime"
    GROUP        "Vocoder"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("vocoder_devel"
    GROUP        "Vocoder"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("vocoder_python"
    GROUP        "Vocoder"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "core_python;vocoder_runtime"
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
    DEPENDS      "core_swig;vocoder_python;vocoder_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(include)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(python)
    ADD_SUBDIRECTORY(examples)
ENDIF(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-vocoder.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "vocoder_devel"
)

ENDIF(ENABLE_GR_VOCODER)
