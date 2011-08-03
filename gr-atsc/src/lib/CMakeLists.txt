# Copyright 2011 Free Software Foundation, Inc.
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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_ATSC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Generate viterbi mux source
########################################################################
ADD_EXECUTABLE(atsci_viterbi_gen atsci_viterbi_gen.cc)

SET(atsci_viterbi_mux_cc ${CMAKE_CURRENT_BINARY_DIR}/atsci_viterbi_mux.cc)

ADD_CUSTOM_COMMAND(
    OUTPUT ${atsci_viterbi_mux_cc}
    DEPENDS atsci_viterbi_gen
    COMMAND atsci_viterbi_gen -o ${atsci_viterbi_mux_cc}
)

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_atsc_sources
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

LIST(APPEND atsc_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-atsc SHARED ${gr_atsc_sources})
TARGET_LINK_LIBRARIES(gnuradio-atsc ${atsc_libs})
SET_TARGET_PROPERTIES(gnuradio-atsc PROPERTIES DEFINE_SYMBOL "gnuradio_atsc_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-atsc PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-atsc
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "atsc_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "atsc_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "atsc_runtime" # .dll file
)

########################################################################
# Build and register unit test
########################################################################
IF(ENABLE_TESTING)

INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
LINK_DIRECTORIES(${CPPUNIT_LIBRARY_DIRS})

LIST(APPEND test_atsci_sources
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
    qa_atsci.cc
    qa_interleaver_fifo.cc
)

INCLUDE(GrTest)
SET(GR_TEST_TARGET_DEPS gnuradio-atsc gnuradio-core)
ADD_EXECUTABLE(test_atsci ${test_atsci_sources} test_atsci.cc)
TARGET_LINK_LIBRARIES(test_atsci gnuradio-atsc gnuradio-core ${CPPUNIT_LIBRARIES})
GR_ADD_TEST(atsci-test test_atsci)

ENDIF(ENABLE_TESTING)

########################################################################
# Install public header files
########################################################################
INSTALL(FILES
    atsc_api.h
    atsc_consts.h
    atsc_derandomizer.h
    atsc_randomizer.h
    atsc_rs_decoder.h
    atsc_rs_encoder.h
    atsc_interleaver.h
    atsc_deinterleaver.h
    atsc_trellis_encoder.h
    atsc_viterbi_decoder.h
    atsc_ds_to_softds.h
    atsc_field_sync_mux.h
    atsc_field_sync_demux.h
    atsc_equalizer.h
    atsc_fs_checker.h
    atsc_bit_timing_loop.h
    atsc_fpll.h
    atsc_depad.h
    atsc_pad.h
    atsc_types.h
    atsci_basic_trellis_encoder.h
    atsci_data_interleaver.h
    atsci_diag_output.h
    atsci_equalizer.h
    atsci_equalizer_lms.h
    atsci_equalizer_lms2.h
    atsci_equalizer_nop.h
    atsci_exp2_lp.h
    atsci_fake_single_viterbi.h
    atsci_fs_checker.h
    atsci_fs_checker_naive.h
    atsci_fs_correlator.h
    atsci_fs_correlator_naive.h
    atsci_pnXXX.h
    atsci_randomizer.h
    atsci_reed_solomon.h
    atsci_root_raised_cosine.h
    atsci_root_raised_cosine_bandpass.h
    atsci_single_viterbi.h
    atsci_slicer_agc.h
    atsci_sliding_correlator.h
    atsci_sssr.h
    atsci_syminfo.h
    atsci_sync_tag.h
    atsci_trellis_encoder.h
    atsci_viterbi_decoder.h
    atsci_vsbtx_lp.h
    convolutional_interleaver.h
    create_atsci_equalizer.h
    create_atsci_fs_checker.h
    create_atsci_fs_correlator.h
    fpll_btloop_coupling.h
    interleaver_fifo.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "atsc_devel"
)

########################################################################
# Setup swig generation
########################################################################
IF(ENABLE_PYTHON)
INCLUDE(GrPython)
INCLUDE(GrSwig)

SET(GR_SWIG_INCLUDE_DIRS
    ${GR_ATSC_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

SET(GR_SWIG_LIBRARIES gnuradio-atsc)

GR_SWIG_MAKE(atsc atsc.i)

GR_SWIG_INSTALL(
    TARGETS atsc
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "atsc_python"
)

INSTALL(
    FILES atsc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "atsc_swig"
)

ENDIF(ENABLE_PYTHON)
