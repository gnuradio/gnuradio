# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


########################################################################
# Setup dependencies
########################################################################
include(GrBoost)
find_package(ZeroMQ)

########################################################################
# Register component
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("gr-zeromq" ENABLE_GR_ZEROMQ
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ZEROMQ_FOUND
)

SET(GR_PKG_ZEROMQ_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/zeromq)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_ZEROMQ)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/zeromq)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/zeromq)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-zeromq.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_ZEROMQ)
