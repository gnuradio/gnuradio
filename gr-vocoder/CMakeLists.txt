# Copyright 2011,2016,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
# ---- crickets

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-vocoder" ENABLE_GR_VOCODER
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_BLOCKS
    ENABLE_GR_FILTER
    ENABLE_GR_ANALOG
)

SET(GR_PKG_VOCODER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/vocoder)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_VOCODER)

########################################################################
## CODEC2 Support
########################################################################
find_package(Codec2)
if(LIBCODEC2_FOUND)
  GR_APPEND_SUBCOMPONENT("codec2")
  #message(STATUS "libcodec2 modes: ${CODEC2_MODES}")
  if(LIBCODEC2_HAS_CODEC2_MODE_700)
    #message(STATUS "libcodec2 has 700bps mode")
  endif()
  if (LIBCODEC2_HAS_FREEDV_API)
    GR_APPEND_SUBCOMPONENT("freedv")
    #message(STATUS "libcodec2 had FreeDV api modes: ${FREEDV_MODES}")
  endif()
endif(LIBCODEC2_FOUND)

########################################################################
## GSM Support
########################################################################
find_package(GSM)
if(LIBGSM_FOUND)
  GR_APPEND_SUBCOMPONENT("gsm")
endif(LIBGSM_FOUND)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/vocoder)
add_subdirectory(lib)
add_subdirectory(docs)
if(ENABLE_PYTHON)
    add_subdirectory(python/vocoder)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-vocoder.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-vocoder.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_VOCODER)
