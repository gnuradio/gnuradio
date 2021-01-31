# Copyright 2021 Jacob Gilbert
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-pdu
    pdu_split_impl.cc
    pdu.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pdu.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.rc
    @ONLY)

    target_sources(gnuradio-pdu PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pdu.rc
    )
endif(MSVC)

target_link_libraries(gnuradio-pdu PUBLIC
    gnuradio-runtime
    gnuradio-blocks
    gnuradio-filter
)

target_include_directories(gnuradio-pdu
  PUBLIC $<INSTALL_INTERFACE:include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )


# we need -no-as-needed or else -lgslcblas gets stripped out on newer version of gcc
if(CMAKE_COMPILER_IS_GNUCC AND NOT APPLE)
    SET_TARGET_PROPERTIES(gnuradio-pdu PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
endif()

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-pdu)
endif()
