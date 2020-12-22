# Copyright 2020 Free Software Foundation, Inc.
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
GR_REGISTER_COMPONENT("gr-message" ENABLE_GR_MESSAGE
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_BLOCKS
    ENABLE_GR_FILTER
    )

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_MESSAGE)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/message)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(python/message)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-message.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-message.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-message.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_MESSAGE)
