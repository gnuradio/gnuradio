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

find_package(UHD "3.5.5")

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-uhd" ENABLE_GR_UHD
    Boost_FOUND
    UHD_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FILTER
    ENABLE_GR_BLOCKS
    ENABLE_GR_ANALOG
)
message(STATUS "  UHD Version: ${UHD_VERSION}")

GR_SET_GLOBAL(GR_UHD_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(GR_PKG_UHD_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/uhd)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_UHD)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/uhd)
add_subdirectory(lib)
add_subdirectory(docs)
add_subdirectory(examples/c++)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/uhd)
    add_subdirectory(apps)
    add_subdirectory(examples/grc)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-uhd.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_UHD)
