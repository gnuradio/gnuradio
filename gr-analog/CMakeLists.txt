# Copyright 2012,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
# Nobody here but us chickens

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component("gr-analog" ENABLE_GR_ANALOG ENABLE_GNURADIO_RUNTIME
                      ENABLE_GR_BLOCKS ENABLE_GR_FFT ENABLE_GR_FILTER)

set(GR_PKG_ANALOG_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/analog)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_ANALOG)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/analog)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_PYTHON)
        add_subdirectory(python/analog)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
            add_subdirectory(examples/tags)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-analog.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-analog.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_ANALOG)
