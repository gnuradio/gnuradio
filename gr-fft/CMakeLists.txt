# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

if(WIN32)
    # Name on Windows does not include datatype
    set(fftw_name FFTW3)
else()
    set(fftw_name FFTW3F)
endif()
gr_find_package(${fftw_name})

########################################################################
# Register component
########################################################################
include(GrComponent)

gr_register_component(
    NAME "gr-fft"
    VAR ENABLE_GR_FFT
    REQUIRED_COMPONENTS
        gnuradio-runtime
        gr-blocks
    REQUIRED_TPLS
        Boost_FOUND
        ${fftw_name}_FOUND
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FFT)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/fft)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/fft)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)
    add_subdirectory(docs)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fft.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_FFT)
