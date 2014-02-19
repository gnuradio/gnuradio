# Copyright 2013 Free Software Foundation, Inc.
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
        build_utils.expand_template(d, inp)

")

macro(expand_h root)
  #make a list of all the generated files
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  
  #create a command to generate the files
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
    )

  #install rules for the generated h files
  list(APPEND generated_includes ${expanded_files_h})
endmacro(expand_h)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_h(add_XX             ss ii cc)
expand_h(add_const_XX       bb ss ii ff cc)
expand_h(add_const_vXX      bb ss ii ff cc)
expand_h(and_XX             bb ss ii)
expand_h(and_const_XX       bb ss ii)
expand_h(argmax_XX          fs is ss)
expand_h(divide_XX          ss ii ff cc)
expand_h(integrate_XX       ss ii ff cc)
expand_h(max_XX             ff ii ss)
expand_h(moving_average_XX  ss ii ff cc)
expand_h(multiply_XX        ss ii)
expand_h(multiply_const_XX  ss ii)
expand_h(multiply_const_vXX ss ii ff cc)
expand_h(mute_XX            ss ii ff cc)
expand_h(not_XX             bb ss ii)
expand_h(or_XX              bb ss ii)
expand_h(peak_detector_XX   fb ib sb)
expand_h(probe_signal_X     b s i f c)
expand_h(probe_signal_vX    b s i f c)
expand_h(sample_and_hold_XX bb ss ii ff)
expand_h(sub_XX             ss ii ff cc)
expand_h(xor_XX             bb ss ii)
expand_h(packed_to_unpacked_XX bb ss ii)
expand_h(unpacked_to_packed_XX bb ss ii)
expand_h(vector_insert_X    b s i f c)
expand_h(vector_sink_X      b s i f c)
expand_h(vector_source_X    b s i f c)

add_custom_target(blocks_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    control_loop.h
    count_bits.h
    file_sink_base.h
    lfsr_15_1_0.h
    lfsr_32k.h
    log2_const.h
    rotator.h
    wavfile.h
    add_ff.h
    annotator_1to1.h
    annotator_alltoall.h
    annotator_raw.h
    bin_statistics_f.h
    burst_tagger.h
    char_to_float.h
    char_to_short.h
    check_lfsr_32k_s.h
    complex_to_interleaved_short.h
    complex_to_float.h
    complex_to_imag.h
    complex_to_real.h
    complex_to_mag.h
    complex_to_mag_squared.h
    complex_to_arg.h
    conjugate_cc.h
    copy.h
    deinterleave.h
    delay.h
    endian_swap.h
    file_descriptor_sink.h
    file_descriptor_source.h
    file_sink.h
    file_source.h
    file_meta_sink.h
    file_meta_source.h
    float_to_char.h
    float_to_complex.h
    float_to_int.h
    float_to_short.h
    float_to_uchar.h
    head.h
    int_to_float.h
    interleave.h
    interleaved_short_to_complex.h
    interleaved_char_to_complex.h
    keep_m_in_n.h
    keep_one_in_n.h
    lfsr_32k_source_s.h
    message_debug.h
    message_sink.h
    message_source.h
    message_strobe.h
    message_strobe_random.h
    message_burst_source.h
    multiply_cc.h
    multiply_ff.h
    multiply_conjugate_cc.h
    multiply_const_cc.h
    multiply_const_ff.h
    nlog10_ff.h
    nop.h
    null_sink.h
    null_source.h
    pack_k_bits_bb.h
    patterned_interleaver.h
    pdu.h
    pdu_to_tagged_stream.h
    peak_detector2_fb.h
    random_pdu.h
    plateau_detector_fb.h
    probe_rate.h
    regenerate_bb.h
    repack_bits_bb.h
    repeat.h
    rms_cf.h
    rms_ff.h
    rotator_cc.h
    short_to_char.h
    short_to_float.h
    skiphead.h
    socket_pdu.h
    stream_mux.h
    stream_to_streams.h
    stream_to_tagged_stream.h
    stream_to_vector.h
    streams_to_stream.h
    streams_to_vector.h
    stretch_ff.h
    tag_debug.h
    tag_gate.h
    tagged_file_sink.h
    tagged_stream_mux.h
    tagged_stream_multiply_length.h
    tagged_stream_to_pdu.h
    tags_strobe.h
    threshold_ff.h
    throttle.h
    transcendental.h
    tuntap_pdu.h
    uchar_to_float.h
    udp_sink.h
    udp_source.h
    unpack_k_bits_bb.h
    vco_f.h
    vco_c.h
    vector_map.h
    vector_to_stream.h
    vector_to_streams.h
    wavfile_sink.h
    wavfile_source.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/blocks
    COMPONENT "blocks_devel"
)

if(ENABLE_GR_CTRLPORT)
install(FILES
  ctrlport_probe_c.h
  ctrlport_probe2_c.h
  ctrlport_probe2_f.h
  ctrlport_probe2_s.h
  ctrlport_probe2_i.h
  ctrlport_probe2_b.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/blocks
  COMPONENT "blocks_devel"
)
endif(ENABLE_GR_CTRLPORT)
