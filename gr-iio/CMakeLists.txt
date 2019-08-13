# Copyright 2011,2012 Free Software Foundation, Inc.
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

find_package(IIO)

set(GR_IIO_DEPENDENCIES
  Boost_FOUND
  ENABLE_GNURADIO_RUNTIME
  ENABLE_GR_BLOCKS
  ENABLE_GR_ANALOG
  ENABLE_VOLK
  IIO_FOUND)

if (NOT LIBAD9361_FOUND STREQUAL "OFF")
  find_package(AD9361)
  list(APPEND GR_IIO_DEPENDENCIES LIBAD9361_FOUND)
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-iio" ENABLE_GR_IIO ${GR_IIO_DEPENDENCIES})

SET(GR_PKG_IIO_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/iio)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_IIO)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/iio)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/iio)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-iio.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_IIO)
