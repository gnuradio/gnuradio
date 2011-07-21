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
GR_REGISTER_COMPONENT("gr-pager" ENABLE_GR_PAGER
    UNIX #FIXME until we get MSVC working
    Boost_FOUND
    ENABLE_GR_CORE_
)

REMOVE_DEFINITIONS(-fvisibility=hidden) #FIXME until we do symbol visibility

GR_SET_GLOBAL(GR_PAGER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_PAGER)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_PAGER_DESCRIPTION "The GNU Radio Pager Blocks")

CPACK_COMPONENT("pager_runtime"
    GROUP        "Pager"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("pager_devel"
    GROUP        "Pager"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("pager_python"
    GROUP        "Pager"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;pager_runtime"
)

CPACK_COMPONENT("pager_swig"
    GROUP        "Pager"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;pager_python;pager_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(python)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(grc)
    ADD_SUBDIRECTORY(apps)
ENDIF(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pager.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pager.pc
@ONLY)

INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pager.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "pager_devel"
)

ENDIF(ENABLE_GR_PAGER)
