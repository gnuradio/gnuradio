# Copyright 2014 Free Software Foundation, Inc.
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
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GR_DTV_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FEC_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Generate viterbi mux source
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
if(NOT CMAKE_CROSSCOMPILING)
  add_executable(atsc_viterbi_gen atsc_viterbi_gen.cc)
  export(TARGETS atsc_viterbi_gen APPEND FILE ${EXPORT_FILE})
endif()

set(atsc_viterbi_mux_cc ${CMAKE_CURRENT_BINARY_DIR}/atsc_viterbi_mux.cc)

add_custom_command(
    OUTPUT ${atsc_viterbi_mux_cc}
    DEPENDS atsc_viterbi_gen
    COMMAND atsc_viterbi_gen -o ${atsc_viterbi_mux_cc}
)

########################################################################
# Setup library
########################################################################
list(APPEND dtv_sources
  ${atsc_viterbi_mux_cc}
  atsc_deinterleaver_impl.cc
  atsc_depad_impl.cc
  atsc_derandomizer_impl.cc
  atsc_equalizer_impl.cc
  atsc_fpll_impl.cc
  atsc_fs_checker_impl.cc
  atsc_randomize.cc
  atsc_rs_decoder_impl.cc
  atsc_sync_impl.cc
  atsc_fake_single_viterbi.cc
  atsc_single_viterbi.cc
  atsc_viterbi_decoder_impl.cc
)

if(ENABLE_GR_CTRLPORT)
list(APPEND dtv_sources
)
endif(ENABLE_GR_CTRLPORT)

list(APPEND dtv_libs
    gnuradio-runtime
    gnuradio-analog
    gnuradio-filter
    gnuradio-fec
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-dtv.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc
    @ONLY)

    list(APPEND dtv_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.rc
    )
endif(MSVC)

add_library(gnuradio-dtv SHARED ${dtv_sources})
target_link_libraries(gnuradio-dtv ${dtv_libs})
GR_LIBRARY_FOO(gnuradio-dtv RUNTIME_COMPONENT "dtv_runtime" DEVEL_COMPONENT "dtv_devel")
