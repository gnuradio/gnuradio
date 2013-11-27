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
    receiver/atsc_derandomizer.cc
    receiver/atsc_rs_decoder.cc
    receiver/atsc_deinterleaver.cc
    receiver/atsc_viterbi_decoder.cc
    receiver/atsc_equalizer.cc
    receiver/atsc_fs_checker.cc
    receiver/atsc_bit_timing_loop.cc
    receiver/atsc_fpll.cc
    receiver/atsc_depad.cc
    receiver/atsci_fake_single_viterbi.cc
    receiver/atsci_single_viterbi.cc
    transmitter/atsc_randomizer.cc
    transmitter/atsc_rs_encoder.cc
    transmitter/atsc_interleaver.cc
    transmitter/atsc_trellis_encoder.cc
    transmitter/atsc_pad.cc
    transmitter/atsci_basic_trellis_encoder.cc
    transmitter/atsci_data_interleaver.cc
    transmitter/atsc_field_sync_mux.cc
    transmitter/atsc_pilot.cc
    transmitter/atsc_ds_to_softds.cc
    transmitter/atsci_randomizer.cc
    transmitter/atsci_trellis_encoder.cc
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

########################################################################
# Build and register unit test
########################################################################
if(ENABLE_TESTING)

include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

list(APPEND test_atsci_sources
    qa/qa_atsci_basic_trellis_encoder.cc
    qa/qa_atsci_data_interleaver.cc
    qa/qa_atsci_fake_single_viterbi.cc
    qa/qa_atsci_single_viterbi.cc
    qa/qa_atsci_randomizer.cc
    qa/qa_atsci_reed_solomon.cc
    qa/qa_atsci_trellis_encoder.cc
    qa/qa_atsci_viterbi_decoder.cc
    qa/qa_convolutional_interleaver.cc
    qa/qa_atsci.cc
    qa/qa_interleaver_fifo.cc
)

include(GrTest)
list(APPEND GR_TEST_TARGET_DEPS gnuradio-atsc gnuradio-fec gnuradio-filter gnuradio-analog gnuradio-fft)
add_executable(test_atsci ${test_atsci_sources} qa/test_atsci.cc)
target_link_libraries(test_atsci gnuradio-atsc gnuradio-runtime ${CPPUNIT_LIBRARIES})
GR_ADD_TEST(atsci-test test_atsci)

endif(ENABLE_TESTING)
