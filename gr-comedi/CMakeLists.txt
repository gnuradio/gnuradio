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

include(FindPkgConfig)

PKG_CHECK_MODULES(COMEDI comedilib)

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-comedi" ENABLE_GR_COMEDI
    COMEDI_FOUND
    LINUX #comedi uses linux specific device noces
    Boost_FOUND
    ENABLE_GR_CORE
)

remove_definitions(-fvisibility=hidden) #FIXME until we do symbol visibility

GR_SET_GLOBAL(GR_COMEDI_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_COMEDI)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_COMEDI_DESCRIPTION "GNU Radio Comedi Blocks")

CPACK_COMPONENT("comedi_runtime"
    GROUP        "Comedi"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("comedi_devel"
    GROUP        "Comedi"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("comedi_python"
    GROUP        "Comedi"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;comedi_runtime"
)

CPACK_COMPONENT("comedi_swig"
    GROUP        "Comedi"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;comedi_python;comedi_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-comedi.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-comedi.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-comedi.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "comedi_devel"
)

endif(ENABLE_GR_COMEDI)
