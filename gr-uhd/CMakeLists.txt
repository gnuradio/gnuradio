# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(UHD "3.9.7")

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
