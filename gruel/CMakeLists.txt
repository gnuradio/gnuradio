# Copyright 2010-2011 Free Software Foundation, Inc.
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

include(GrPython) #used for code generation

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gruel" ENABLE_GRUEL
    Boost_FOUND
    PYTHONINTERP_FOUND
)

include(GrMiscUtils)
GR_SET_GLOBAL(GRUEL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_BINARY_DIR}/src/include
    ${Boost_INCLUDE_DIRS}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GRUEL)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_GRUEL_DESCRIPTION "GNU Radio Utility Etcetera Library")

CPACK_COMPONENT("gruel_runtime"
    GROUP        "Gruel"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
)

CPACK_COMPONENT("gruel_devel"
    GROUP        "Gruel"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
)

CPACK_COMPONENT("gruel_python"
    GROUP        "Gruel"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS     "gruel_runtime"
)

CPACK_COMPONENT("gruel_swig"
    GROUP        "Gruel"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS     "gruel_python;gruel_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src/include/gruel)
add_subdirectory(src/lib)
if(ENABLE_PYTHON)
    add_subdirectory(src/swig)
    add_subdirectory(src/python)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gruel.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gruel.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gruel.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "gruel_devel"
)

endif(ENABLE_GRUEL)
