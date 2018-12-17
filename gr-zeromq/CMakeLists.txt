# Copyright 2013 Free Software Foundation, Inc.
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
find_package(ZeroMQ)

########################################################################
# Register component
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("gr-zeromq" ENABLE_GR_ZEROMQ
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ZEROMQ_FOUND
)

GR_SET_GLOBAL(GR_ZEROMQ_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(GR_PKG_ZEROMQ_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/zeromq)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_ZEROMQ)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/zeromq)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/zeromq)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-zeromq.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_ZEROMQ)
