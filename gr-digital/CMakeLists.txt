# Copyright 2011,2019 Free Software Foundation, Inc.
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
gr_register_component(
    "gr-digital"
    ENABLE_GR_DIGITAL
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_FILTER
    ENABLE_GR_BLOCKS
    ENABLE_GR_ANALOG)

set(GR_PKG_DIGITAL_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/digital)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_DIGITAL)

    # Set HAVE_BUILTIN_PARITYL for use in scrambler if compiler is GNU
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_definitions(-DHAVE_BUILTIN_PARITYL)
    endif()

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/digital)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_PYTHON)
        add_subdirectory(python/digital)
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
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-digital.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-digital.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_DIGITAL)
