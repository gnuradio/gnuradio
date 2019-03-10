# Copyright 2012,2014,2018-2019 Free Software Foundation, Inc.
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
add_library(gnuradio-trellis
    base.cc
    calc_metric.cc
    core_algorithms.cc
    encoder_impl.cc
    fsm.cc
    interleaver.cc
    quicksort_index.cc
    constellation_metrics_cf_impl.cc
    metrics_impl.cc
    pccc_decoder_blk_impl.cc
    pccc_encoder_impl.cc
    pccc_decoder_combined_blk_impl.cc
    permutation_impl.cc
    sccc_decoder_blk_impl.cc
    sccc_decoder_combined_blk_impl.cc
    sccc_encoder_impl.cc
    siso_f_impl.cc
    siso_combined_f_impl.cc
    viterbi_impl.cc
    viterbi_combined_impl.cc
)

target_link_libraries(gnuradio-trellis PUBLIC
    gnuradio-runtime
    gnuradio-digital
)

target_include_directories(gnuradio-trellis
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-trellis.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc
    @ONLY)

    target_sources(gnuradio-trellis PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc
    )
ENDIF(MSVC)

if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-trellis)
endif()
