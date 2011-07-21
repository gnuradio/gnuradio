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
GR_REGISTER_COMPONENT("gr-digital" ENABLE_GR_DIGITAL
    Boost_FOUND
    ENABLE_GR_CORE_
)

GR_SET_GLOBAL(GR_DIGITAL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

GR_SET_GLOBAL(GR_DIGITAL_SWIG_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/swig
)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_DIGITAL)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_TRELLIS_DESCRIPTION "The GNU Radio Digital Blocks")

CPACK_COMPONENT("digital_runtime"
    GROUP        "Digital"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("digital_devel"
    GROUP        "Digital"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("digital_python"
    GROUP        "Digital"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "core_python;digital_runtime"
)

CPACK_COMPONENT("digital_swig"
    GROUP        "Digital"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;digital_python;digital_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(python)
    ADD_SUBDIRECTORY(grc)
ENDIF(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-digital.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "digital_devel"
)

ENDIF(ENABLE_GR_DIGITAL)
