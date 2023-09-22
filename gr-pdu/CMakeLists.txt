# Copyright 2021 Jacob Gilbert
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
gr_register_component("gr-pdu" ENABLE_GR_PDU ENABLE_GNURADIO_RUNTIME)

set(GR_PKG_PDU_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/pdu)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_PDU)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/pdu)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/pdu)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pdu.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_PDU)
