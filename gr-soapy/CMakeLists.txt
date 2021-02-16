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

########################################################################
# External dependencies 
########################################################################
find_package(SoapySDR REQUIRED)

if(NOT SoapySDR_FOUND)
    message(FATAL_ERROR "SoapySDR required to compile gr-soapy")
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-soapy" ENABLE_GR_SOAPY
    Boost_FOUND
    SOAPYSDR_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FILTER
    ENABLE_GR_BLOCKS
    ENABLE_GR_ANALOG
)
SET(GR_PKG_UHD_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/uhd)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_SOAPY)
message(STATUS "  SOAPY Version: ${SOAPY_VERSION}")

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/soapy)
add_subdirectory(lib)
add_subdirectory(docs)
if(ENABLE_EXAMPLES)
    add_subdirectory(examples/grc)
endif(ENABLE_EXAMPLES)
if(ENABLE_PYTHON)
    add_subdirectory(python/soapy)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-soapy.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-soapy.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-soapy.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_SOAPY)
