# Copyright 2012,2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-wavelet
    squash_ff_impl.cc
    wavelet_ff_impl.cc
    wvps_ff_impl.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-wavelet.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.rc
    @ONLY)

    target_sources(gnuradio-wavelet PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-wavelet.rc
    )
endif(MSVC)

target_link_libraries(gnuradio-wavelet PUBLIC
    gnuradio-runtime
    GSL::gsl
)

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-wavelet PRIVATE common-precompiled-headers)
endif()

target_include_directories(gnuradio-wavelet
  PUBLIC $<INSTALL_INTERFACE:include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )


# we need -no-as-needed or else -lgslcblas gets stripped out on newer version of gcc
if(CMAKE_COMPILER_IS_GNUCC AND NOT APPLE)
    SET_TARGET_PROPERTIES(gnuradio-wavelet PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
endif()

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-wavelet)
endif()
