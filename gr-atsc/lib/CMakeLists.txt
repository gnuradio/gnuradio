# Copyright 2011-2013 Free Software Foundation, Inc.
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
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GR_ATSC_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${GR_FEC_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${LOG4CPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${LOG4CPP_LIBRARY_DIRS})

########################################################################
# Generate viterbi mux source
# http://www.vtk.org/Wiki/CMake_Cross_Compiling#Using_executables_in_the_build_created_during_the_build
########################################################################
if(NOT CMAKE_CROSSCOMPILING)
    add_executable(atsci_viterbi_gen atsci_viterbi_gen.cc)
    export(TARGETS atsci_viterbi_gen APPEND FILE ${EXPORT_FILE})
endif()


set(atsci_viterbi_mux_cc ${CMAKE_CURRENT_BINARY_DIR}/atsci_viterbi_mux.cc)

add_custom_command(
    OUTPUT ${atsci_viterbi_mux_cc}
    DEPENDS atsci_viterbi_gen
    COMMAND atsci_viterbi_gen -o ${atsci_viterbi_mux_cc}
)

########################################################################
# Setup library
########################################################################
list(APPEND gr_atsc_sources
    ${atsci_viterbi_mux_cc}
    atsc_derandomizer.cc
    atsc_randomizer.cc
    atsc_rs_decoder.cc
    atsc_rs_encoder.cc
    atsc_interleaver.cc
    atsc_deinterleaver.cc
    atsc_trellis_encoder.cc
    atsc_viterbi_decoder.cc
    atsc_ds_to_softds.cc
    atsc_field_sync_mux.cc
    atsc_field_sync_demux.cc
    atsc_equalizer.cc
    atsc_fs_checker.cc
    atsc_bit_timing_loop.cc
    atsc_fpll.cc
    atsc_depad.cc
    atsc_pad.cc
    atsci_basic_trellis_encoder.cc
    atsci_data_interleaver.cc
    atsci_equalizer.cc
    atsci_equalizer_lms.cc
    atsci_equalizer_lms2.cc
    atsci_equalizer_nop.cc
    atsci_fake_single_viterbi.cc
    atsci_fs_checker.cc
    atsci_fs_checker_naive.cc
    atsci_fs_correlator.cc
    atsci_fs_correlator_naive.cc
    atsci_single_viterbi.cc
    atsci_sssr.cc
    atsci_pnXXX.cc
    atsci_randomizer.cc
    atsci_reed_solomon.cc
    atsci_sliding_correlator.cc
    atsci_trellis_encoder.cc
    atsci_viterbi_decoder.cc
    create_atsci_equalizer.cc
    create_atsci_fs_checker.cc
    create_atsci_fs_correlator.cc
    plinfo.cc
)

#Add Windows DLL resource file under MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-atsc.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-atsc.rc
    @ONLY)

    list(APPEND gr_atsc_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-atsc.rc
    )
ENDIF(MSVC)

list(APPEND atsc_libs
    gnuradio-filter
    gnuradio-analog
    gnuradio-runtime
    gnuradio-fec
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

add_library(gnuradio-atsc SHARED ${gr_atsc_sources})
target_link_libraries(gnuradio-atsc ${atsc_libs})
GR_LIBRARY_FOO(gnuradio-atsc RUNTIME_COMPONENT "atsc_runtime" DEVEL_COMPONENT "atsc_devel")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-atsc APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-atsc_static STATIC ${gr_atsc_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-atsc_static
      PROPERTIES OUTPUT_NAME gnuradio-atsc)
  endif(NOT WIN32)

  install(TARGETS gnuradio-atsc_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "atsc_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

########################################################################
# Build and register unit test
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
  )

  list(APPEND test_gr_atsc_source
      qa_atsci_basic_trellis_encoder.cc
      qa_atsci_data_interleaver.cc
      qa_atsci_equalizer_nop.cc
      qa_atsci_fake_single_viterbi.cc
      qa_atsci_fs_correlator.cc
      qa_atsci_single_viterbi.cc
      qa_atsci_randomizer.cc
      qa_atsci_reed_solomon.cc
      qa_atsci_sliding_correlator.cc
      qa_atsci_trellis_encoder.cc
      qa_atsci_viterbi_decoder.cc
      qa_convolutional_interleaver.cc
      qa_interleaver_fifo.cc
  )
  list(APPEND GR_TEST_TARGET_DEPS
    gnuradio-atsc
    gnuradio-fec
    gnuradio-filter
    gnuradio-analog
    gnuradio-fft
  )

  foreach(qa_file ${test_gr_atsc_source})
    GR_ADD_CPP_TEST("atsc_${qa_file}"
      ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
  endforeach(qa_file)

endif(ENABLE_TESTING)
