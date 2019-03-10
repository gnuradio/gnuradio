# Copyright 2012,2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

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
    gnuradio-blocks
    gsl::gsl
)

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
