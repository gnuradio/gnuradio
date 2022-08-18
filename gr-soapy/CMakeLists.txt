# Copyright 2011-2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################

########################################################################
# External dependencies
########################################################################
find_package(SoapySDR 0.7.2)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component("gr-soapy" ENABLE_GR_SOAPY SoapySDR_FOUND ENABLE_GNURADIO_RUNTIME)
set(GR_PKG_SOAPY_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/soapy)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_SOAPY)
    message(STATUS "  SOAPY Version: ${SoapySDR_VERSION}")

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/soapy)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_PYTHON)
        add_subdirectory(python/soapy)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples/grc)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-soapy.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-soapy.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-soapy.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_SOAPY)
