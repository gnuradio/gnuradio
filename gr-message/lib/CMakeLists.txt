# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-message
    port_names.cc
    message_counter_impl.cc
    message_emitter_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-message.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-message.rc
    @ONLY)

    target_sources(gnuradio-message PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-message.rc
    )
endif(MSVC)

target_link_libraries(gnuradio-message PUBLIC
    gnuradio-runtime
    gnuradio-blocks
)

target_include_directories(gnuradio-message
  PUBLIC $<INSTALL_INTERFACE:include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )


# we need -no-as-needed or else -lgslcblas gets stripped out on newer version of gcc
if(CMAKE_COMPILER_IS_GNUCC AND NOT APPLE)
    SET_TARGET_PROPERTIES(gnuradio-message PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
endif()

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-message)
endif()
