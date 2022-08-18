# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(UHD "3.9.7")
if("${UHD_VERSION}" VERSION_GREATER_EQUAL "4")
    set(UHD_FOUR_POINT_OH_RFNOC TRUE)
else()
    set(UHD_FOUR_POINT_OH_RFNOC FALSE)
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component(
    "gr-uhd"
    ENABLE_GR_UHD
    Boost_FOUND
    UHD_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FILTER
    ENABLE_GR_BLOCKS
    ENABLE_GR_ANALOG)
set(GR_PKG_UHD_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/uhd)

gr_register_component("gr-uhd UHD 4.0 RFNoC" ENABLE_UHD_RFNOC ENABLE_GR_UHD
                      UHD_FOUR_POINT_OH_RFNOC)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_UHD)
    message(STATUS "  UHD Version: ${UHD_VERSION}")

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/uhd)
    add_subdirectory(lib)
    add_subdirectory(docs)
    if(ENABLE_EXAMPLES)
        add_subdirectory(examples/c++)
    endif(ENABLE_EXAMPLES)
    if(ENABLE_PYTHON)
        add_subdirectory(python/uhd)
        add_subdirectory(apps)
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
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-uhd.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-uhd.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_UHD)
