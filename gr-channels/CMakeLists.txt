# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################

########################################################################
# Register component
########################################################################
include(GrComponent)

gr_register_component(
    "gr-channels"
    ENABLE_GR_CHANNELS
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
    ENABLE_GR_ANALOG)

set(GR_PKG_CHANNELS_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/channels)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_CHANNELS)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/channels)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/channels)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)
    add_subdirectory(docs)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-channels.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-channels.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_CHANNELS)
