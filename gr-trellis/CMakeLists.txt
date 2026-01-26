# Copyright 2012,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
# ↺↺↺↺↺↺↺ (Tumbleweed)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component(
    NAME "gr-trellis"
    VAR ENABLE_GR_TRELLIS
    REQUIRED_COMPONENTS gnuradio-runtime
        gr-analog gr-blocks gr-digital
)

set(GR_PKG_TRELLIS_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/trellis)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_TRELLIS)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/trellis)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_PYTHON)
        add_subdirectory(python/trellis)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples/python)
            add_subdirectory(examples/grc)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-trellis.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_TRELLIS)
