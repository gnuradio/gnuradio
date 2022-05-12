# Copyright 2011 Free Software Foundation, Inc.
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
GR_REGISTER_COMPONENT("gr-audio" ENABLE_GR_AUDIO
    ENABLE_GNURADIO_RUNTIME
)

SET(GR_PKG_AUDIO_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/audio)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_AUDIO)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(lib)
add_subdirectory(include/gnuradio/audio)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(python/audio)
    if(ENABLE_EXAMPLES)
        add_subdirectory(examples/python)
        add_subdirectory(examples/grc)
    endif(ENABLE_EXAMPLES)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)
if((ENABLE_GR_ANALOG) AND (ENABLE_EXAMPLES))
  add_subdirectory(examples/c++)
endif((ENABLE_GR_ANALOG) AND (ENABLE_EXAMPLES))

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-audio.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_AUDIO)
