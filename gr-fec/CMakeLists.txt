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

find_package(GSL)

gr_register_component("gr-fec" ENABLE_GR_FEC ENABLE_GNURADIO_RUNTIME ENABLE_GR_BLOCKS)

set(GR_PKG_FEC_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/fec)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FEC)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/fec)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/fec)
        add_subdirectory(python/fec/LDPC)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)
    add_subdirectory(docs)
    add_subdirectory(ldpc_alist)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fec.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fec.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_FEC)
