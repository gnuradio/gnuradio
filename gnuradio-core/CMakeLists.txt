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

find_package(GSL)

find_package(FFTW3f)

include(GrPython) #used for code generation

get_filename_component(GR_CORE_PYTHONPATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/python ABSOLUTE
)
GR_SET_GLOBAL(GR_CORE_PYTHONPATH ${GR_CORE_PYTHONPATH})

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gnuradio-core" ENABLE_GR_CORE
    ENABLE_GRUEL
    ENABLE_VOLK
    Boost_FOUND
    GSL_FOUND
    FFTW3F_FOUND
    PYTHONINTERP_FOUND
)

include(GrMiscUtils)
GR_SET_GLOBAL(GNURADIO_CORE_INCLUDE_DIRS
    ${GRUEL_INCLUDE_DIRS} #headers depend on gruel
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/runtime
    ${CMAKE_CURRENT_BINARY_DIR}/src/lib/general
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/general
    ${CMAKE_CURRENT_BINARY_DIR}/src/lib/gengen
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/gengen
    ${CMAKE_CURRENT_BINARY_DIR}/src/lib/filter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/filter
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/missing
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/reed-solomon
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/viterbi
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/io
    ${CMAKE_CURRENT_BINARY_DIR}/src/lib/swig
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/swig
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/hier
)

GR_SET_GLOBAL(GNURADIO_CORE_SWIG_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/gruel/src/swig
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/swig
    ${GNURADIO_CORE_INCLUDE_DIRS}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_CORE)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_CORE_DESCRIPTION "GNU Radio Core Library")

CPACK_COMPONENT("core_runtime"
    GROUP        "Core"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "gruel_runtime"
)

CPACK_COMPONENT("core_devel"
    GROUP        "Core"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "gruel_devel"
)

CPACK_COMPONENT("core_python"
    GROUP        "Core"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "gruel_python;core_runtime"
)

CPACK_COMPONENT("core_swig"
    GROUP        "Core"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "gruel_swig;core_python;core_devel"
)

install(
    FILES gnuradio-core.conf
    DESTINATION ${GR_PKG_CONF_DIR}
    COMPONENT "core_runtime"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src/lib)
if(ENABLE_TESTING)
    add_subdirectory(src/tests)
endif(ENABLE_TESTING)
if(ENABLE_PYTHON)
    add_subdirectory(src/lib/swig)
    add_subdirectory(src/python/gnuradio)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-core.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-core.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-core.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "core_devel"
)

endif(ENABLE_GR_CORE)
