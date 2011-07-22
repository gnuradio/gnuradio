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
GR_REGISTER_COMPONENT("gr-noaa" ENABLE_GR_NOAA
    Boost_FOUND
    ENABLE_GR_CORE_
)

GR_SET_GLOBAL(GR_NOAA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

########################################################################
# Begin conditional configuration
########################################################################
IF(ENABLE_GR_NOAA)

########################################################################
# Setup CPack components
########################################################################
INCLUDE(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_NOAA_DESCRIPTION "The GNU Radio NOAA Blocks")

CPACK_COMPONENT("noaa_runtime"
    GROUP        "NOAA"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "core_runtime"
)

CPACK_COMPONENT("noaa_devel"
    GROUP        "NOAA"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "core_devel"
)

CPACK_COMPONENT("noaa_python"
    GROUP        "NOAA"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "core_python;noaa_runtime"
)

CPACK_COMPONENT("noaa_swig"
    GROUP        "NOAA"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "core_swig;noaa_python;noaa_devel"
)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
IF(ENABLE_PYTHON)
    ADD_SUBDIRECTORY(swig)
    ADD_SUBDIRECTORY(grc)
    ADD_SUBDIRECTORY(apps)
ENDIF(ENABLE_PYTHON)

ENDIF(ENABLE_GR_NOAA)
