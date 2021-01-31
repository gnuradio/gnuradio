# Copyright 2021 Jacob Gilbert
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
GR_REGISTER_COMPONENT("gr-pdu" ENABLE_GR_PDU
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
    ENABLE_GR_FILTER
    )

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
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pdu.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_PDU)
