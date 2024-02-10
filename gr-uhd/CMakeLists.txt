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

gr_find_package(UHD "3.9.7")
if(UHD_FOUND AND NOT TARGET UHD::UHD)
    add_library(UHD::UHD INTERFACE IMPORTED GLOBAL)
    set_target_properties(
        UHD::UHD PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${UHD_INCLUDE_DIRS}"
                            INTERFACE_LINK_LIBRARIES "${UHD_LIBRARIES}")
endif()

if("${UHD_VERSION}" VERSION_GREATER_EQUAL "4")
    set(UHD_FOUR_POINT_OH_RFNOC TRUE)
else()
    set(UHD_FOUR_POINT_OH_RFNOC FALSE)
endif()

gr_python_check_module("PyQt5" PyQt5 True PYQT5_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component(
    NAME "gr-uhd"
    VAR ENABLE_GR_UHD
    REQUIRED_TPLS
        Boost_FOUND
        UHD_FOUND
    REQUIRED_COMPONENTS
        gnuradio-runtime
        gr-filter
        gr-blocks
        gr-analog
)
set(GR_PKG_UHD_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/uhd)

gr_register_component(
    NAME "gr-uhd UHD 4.0 RFNoC"
    VAR ENABLE_UHD_RFNOC
    NO_INSTALL
    REQUIRED_COMPONENTS ENABLE_GR_UHD
    REQUIRED_TPLS UHD_FOUR_POINT_OH_RFNOC
)

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
