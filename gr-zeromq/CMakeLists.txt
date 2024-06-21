# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


########################################################################
# Setup dependencies
########################################################################
gr_find_package(ZeroMQ)
add_library(ZeroMQ::ZeroMQ INTERFACE IMPORTED)
set_target_properties(
    ZeroMQ::ZeroMQ PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ZeroMQ_INCLUDE_DIRS}
                                INTERFACE_LINK_LIBRARIES ${ZeroMQ_LIBRARIES})
########################################################################
# Register component
########################################################################
include(GrComponent)

gr_register_component(
    NAME "gr-zeromq"
    VAR ENABLE_GR_ZEROMQ
    REQUIRED_COMPONENTS
        gnuradio-runtime
    REQUIRED_TPLS
        # ZEROMQ_FOUND
        ZeroMQ_FOUND
        CPPZMQ_HAS_SHUTDOWN
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
    add_subdirectory(python/zeromq)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-zeromq.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-zeromq.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_ZEROMQ)
