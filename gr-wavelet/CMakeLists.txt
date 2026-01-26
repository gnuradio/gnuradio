# Copyright 2012,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
gr_find_package(GSL)

########################################################################
# Register component
########################################################################
include(GrComponent)

gr_register_component(
    NAME "gr-wavelet"
    VAR ENABLE_GR_WAVELET
    REQUIRED_COMPONENTS
        gnuradio-runtime
        gr-blocks
        gr-analog
    REQUIRED_TPLS
        GSL_FOUND
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_WAVELET)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/wavelet)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(python/wavelet)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-wavelet.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_WAVELET)
