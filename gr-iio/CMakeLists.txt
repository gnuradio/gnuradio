# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(AD9361 QUIET)
find_package(IIO)

set(GR_IIO_DEPENDENCIES
  Boost_FOUND
  ENABLE_GNURADIO_RUNTIME
  ENABLE_GR_BLOCKS
  LIBIIO_FOUND)

if (LIBAD9361_FOUND)
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
