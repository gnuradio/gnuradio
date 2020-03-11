# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

########################################################################
# Register component
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("gr-filter" ENABLE_GR_FILTER
    ENABLE_VOLK
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_BLOCKS
)

SET(GR_PKG_FILTER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/filter)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FILTER)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/filter)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/filter)
    add_subdirectory(python/filter/design)
    add_subdirectory(python/filter/gui)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)
add_subdirectory(examples)
add_subdirectory(docs)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_FILTER)
