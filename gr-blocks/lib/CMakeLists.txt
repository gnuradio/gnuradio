# Copyright 2012-2013 Free Software Foundation, Inc.
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
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)

########################################################################
# generate helper scripts to expand templated files
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict(name, sig, 'blocks')
        build_utils.expand_template(d, inp, '_impl')
")

macro(expand_cc_h_impl root)
  #make a list of all the generated files
  unset(expanded_files_cc_impl)
  unset(expanded_files_h_impl)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.cc)
    list(APPEND expanded_files_h_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.h)
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/../include/${name}.h)
  endforeach(sig)
  
  #create a command to generate the _impl.cc files
  add_custom_command(
    OUTPUT ${expanded_files_cc_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.cc.t ${ARGN}
  )
  
  #create a command to generate the _impl.h files
  add_custom_command(
    OUTPUT ${expanded_files_h_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.h.t ${ARGN}
  )
  
  #make _impl.cc source files depend on headers to force generation
  set_source_files_properties(${expanded_files_cc_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h_impl}"
  )
  
  #make _impl.h source files depend on headers to force generation
  set_source_files_properties(${expanded_files_h_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )

  #install rules for the generated cc files
  list(APPEND generated_sources ${expanded_files_cc_impl})  
endmacro(expand_cc_h_impl)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_cc_h_impl(add_XX             ss ii cc)
expand_cc_h_impl(add_const_XX       bb ss ii ff cc)
expand_cc_h_impl(add_const_vXX      bb ss ii ff cc)
expand_cc_h_impl(and_XX             bb ss ii)
expand_cc_h_impl(and_const_XX       bb ss ii)
expand_cc_h_impl(argmax_XX          fs is ss)
expand_cc_h_impl(divide_XX          ss ii ff cc)
expand_cc_h_impl(integrate_XX       ss ii ff cc)
expand_cc_h_impl(max_XX             ff ii ss)
expand_cc_h_impl(moving_average_XX  ss ii ff cc)
expand_cc_h_impl(multiply_XX        ss ii)
expand_cc_h_impl(multiply_const_XX  ss ii)
expand_cc_h_impl(multiply_const_vXX ss ii ff cc)
expand_cc_h_impl(mute_XX            ss ii ff cc)
expand_cc_h_impl(not_XX             bb ss ii)
expand_cc_h_impl(or_XX              bb ss ii)
expand_cc_h_impl(peak_detector_XX   fb ib sb)
expand_cc_h_impl(probe_signal_X     b s i f c)
expand_cc_h_impl(probe_signal_vX    b s i f c)
expand_cc_h_impl(sample_and_hold_XX bb ss ii ff)
expand_cc_h_impl(sub_XX             ss ii ff cc)
expand_cc_h_impl(xor_XX             bb ss ii)
expand_cc_h_impl(packed_to_unpacked_XX bb ss ii)
expand_cc_h_impl(unpacked_to_packed_XX bb ss ii)
expand_cc_h_impl(vector_insert_X    b s i f c)
expand_cc_h_impl(vector_sink_X      b s i f c)
expand_cc_h_impl(vector_source_X    b s i f c)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND gr_blocks_sources
    ${generated_sources}
    control_loop.cc
    count_bits.cc
    file_sink_base.cc
    wavfile.cc
    add_ff_impl.cc
    annotator_1to1_impl.cc
    annotator_alltoall_impl.cc
    annotator_raw_impl.cc
    bin_statistics_f_impl.cc
    burst_tagger_impl.cc
    char_to_float_impl.cc
    char_to_short_impl.cc
    check_lfsr_32k_s_impl.cc
    complex_to_interleaved_short_impl.cc
    complex_to_float_impl.cc
    complex_to_real_impl.cc
    complex_to_imag_impl.cc
    complex_to_mag_impl.cc
    complex_to_mag_squared_impl.cc
    complex_to_arg_impl.cc
    conjugate_cc_impl.cc
    copy_impl.cc
    deinterleave_impl.cc
    delay_impl.cc
    endian_swap_impl.cc
    file_descriptor_sink_impl.cc
    file_descriptor_source_impl.cc
    file_sink_impl.cc
    file_source_impl.cc
    file_meta_sink_impl.cc
    file_meta_source_impl.cc
    float_to_char_impl.cc
    float_to_complex_impl.cc
    float_array_to_int.cc
    float_to_int_impl.cc
    float_to_short_impl.cc
    float_array_to_uchar.cc
    float_to_uchar_impl.cc
    head_impl.cc
    int_to_float_impl.cc
    interleave_impl.cc
    interleaved_short_array_to_complex.cc
    interleaved_short_to_complex_impl.cc
    interleaved_char_to_complex_impl.cc
    keep_m_in_n_impl.cc
    keep_one_in_n_impl.cc
    lfsr_32k_source_s_impl.cc
    message_debug_impl.cc
    message_sink_impl.cc
    message_source_impl.cc
    message_strobe_impl.cc
    message_strobe_random_impl.cc
    message_burst_source_impl.cc
    multiply_cc_impl.cc
    multiply_ff_impl.cc
    multiply_conjugate_cc_impl.cc
    multiply_const_cc_impl.cc
    multiply_const_ff_impl.cc
    nlog10_ff_impl.cc
    nop_impl.cc
    null_sink_impl.cc
    null_source_impl.cc
    pack_k_bits_bb_impl.cc
    patterned_interleaver_impl.cc
    pdu.cc
    tag_debug_impl.cc
    pdu_to_tagged_stream_impl.cc
    peak_detector2_fb_impl.cc
    random_pdu_impl.cc
    plateau_detector_fb_impl.cc
    probe_rate_impl.cc
    regenerate_bb_impl.cc
    repack_bits_bb_impl.cc
    repeat_impl.cc
    rms_cf_impl.cc
    rms_ff_impl.cc
    rotator_cc_impl.cc
    short_to_char_impl.cc
    short_to_float_impl.cc
    skiphead_impl.cc
    socket_pdu_impl.cc
    stream_mux_impl.cc
    stream_pdu_base.cc
    stream_to_streams_impl.cc
    stream_to_tagged_stream_impl.cc
    stream_to_vector_impl.cc
    streams_to_stream_impl.cc
    streams_to_vector_impl.cc
    stretch_ff_impl.cc
    tagged_file_sink_impl.cc
    tagged_stream_to_pdu_impl.cc
    tagged_stream_multiply_length_impl.cc
    tags_strobe_impl.cc
    threshold_ff_impl.cc
    throttle_impl.cc
    transcendental_impl.cc
    tcp_connection.cc
    tuntap_pdu_impl.cc
    tag_gate_impl.cc
    tagged_stream_mux_impl.cc
    uchar_array_to_float.cc
    uchar_to_float_impl.cc
    udp_sink_impl.cc
    udp_source_impl.cc
    unpack_k_bits_bb_impl.cc
    vco_f_impl.cc
    vco_c_impl.cc
    vector_map_impl.cc
    vector_to_stream_impl.cc
    vector_to_streams_impl.cc
    wavfile_sink_impl.cc
    wavfile_source_impl.cc
)

if(ENABLE_GR_CTRLPORT)
list(APPEND gr_blocks_sources
  ctrlport_probe_c_impl.cc
  ctrlport_probe2_c_impl.cc
  ctrlport_probe2_f_impl.cc
  ctrlport_probe2_s_impl.cc
  ctrlport_probe2_i_impl.cc
  ctrlport_probe2_b_impl.cc
)
endif(ENABLE_GR_CTRLPORT)

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-blocks.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.rc
    @ONLY)

    list(APPEND gr_blocks_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.rc
    )
ENDIF(MSVC)

list(APPEND blocks_libs
    gnuradio-runtime
    volk
    ${Boost_LIBRARIES}
    ${BLOCKS_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

add_library(gnuradio-blocks SHARED ${gr_blocks_sources})
add_dependencies(gnuradio-blocks blocks_generated_includes)

target_link_libraries(gnuradio-blocks ${blocks_libs})
GR_LIBRARY_FOO(gnuradio-blocks RUNTIME_COMPONENT "blocks_runtime" DEVEL_COMPONENT "blocks_devel")

########################################################################
# QA C++ Code for gr-blocks
########################################################################
if(ENABLE_TESTING)
  include(GrTest)

  include_directories(
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIRS})
  link_directories(${CPPUNIT_LIBRARY_DIRS})

  list(APPEND test_gr_blocks_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_gr_blocks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_top_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hier_block2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hier_block2_derived.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_blocks.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_block_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_rotator.cc
    )

  add_executable(test-gr-blocks ${test_gr_blocks_sources})

  list(APPEND GR_TEST_TARGET_DEPS test-gr-blocks gnuradio-blocks)

  target_link_libraries(
    test-gr-blocks
    gnuradio-runtime
    gnuradio-blocks
    ${Boost_LIBRARIES}
    ${CPPUNIT_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
  )

  GR_ADD_TEST(test_gr_blocks test-gr-blocks)
endif(ENABLE_TESTING)
