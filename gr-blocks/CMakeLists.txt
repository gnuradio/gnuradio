# Copyright 2012 Free Software Foundation, Inc.
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
GR_REGISTER_COMPONENT("gr-blocks" ENABLE_GR_BLOCKS
    Boost_FOUND
    ENABLE_VOLK
    ENABLE_GNURADIO_RUNTIME
)

GR_SET_GLOBAL(GR_BLOCKS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${VOLK_INCLUDE_DIRS}
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_BLOCKS)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/blocks)
add_subdirectory(lib)
#if(ENABLE_TESTING)
#  add_subdirectory(tests)
#endif(ENABLE_TESTING)
if(ENABLE_PYTHON)
     add_subdirectory(python/blocks)
     add_subdirectory(swig)
     add_subdirectory(docs)
     add_subdirectory(examples)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-blocks.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_BLOCKS)
