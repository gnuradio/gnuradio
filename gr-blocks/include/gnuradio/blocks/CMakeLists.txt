# Copyright 2013-2014 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(blocks abs_XX             ss ii ff)
GR_EXPAND_X_H(blocks add_XX             ss ii cc)
GR_EXPAND_X_H(blocks add_const_vXX      bb ss ii ff cc)
GR_EXPAND_X_H(blocks and_XX             bb ss ii)
GR_EXPAND_X_H(blocks and_const_XX       bb ss ii)
GR_EXPAND_X_H(blocks argmax_XX          fs is ss)
GR_EXPAND_X_H(blocks divide_XX          ss ii ff cc)
GR_EXPAND_X_H(blocks integrate_XX       ss ii ff cc)
GR_EXPAND_X_H(blocks max_XX             ss ii ff)
GR_EXPAND_X_H(blocks min_XX             ss ii ff)
GR_EXPAND_X_H(blocks moving_average_XX  ss ii ff cc)
GR_EXPAND_X_H(blocks multiply_XX        ss ii)
GR_EXPAND_X_H(blocks multiply_const_XX  ss ii)
GR_EXPAND_X_H(blocks multiply_const_vXX ss ii)
GR_EXPAND_X_H(blocks multiply_matrix_XX ff cc)
GR_EXPAND_X_H(blocks mute_XX            ss ii ff cc)
GR_EXPAND_X_H(blocks not_XX             bb ss ii)
GR_EXPAND_X_H(blocks or_XX              bb ss ii)
GR_EXPAND_X_H(blocks peak_detector_XX   fb ib sb)
GR_EXPAND_X_H(blocks probe_signal_X     b s i f c)
GR_EXPAND_X_H(blocks probe_signal_vX    b s i f c)
GR_EXPAND_X_H(blocks sample_and_hold_XX bb ss ii ff)
GR_EXPAND_X_H(blocks sub_XX             ss ii cc)
GR_EXPAND_X_H(blocks tsb_vector_sink_X  b s i f c)
GR_EXPAND_X_H(blocks xor_XX             bb ss ii)
GR_EXPAND_X_H(blocks packed_to_unpacked_XX bb ss ii)
GR_EXPAND_X_H(blocks unpacked_to_packed_XX bb ss ii)
GR_EXPAND_X_H(blocks vector_insert_X    b s i f c)
GR_EXPAND_X_H(blocks vector_sink_X      b s i f c)
GR_EXPAND_X_H(blocks vector_source_X    b s i f c)

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
    pack_k_bits.h
    rotator.h
    unpack_k_bits.h
    wavfile.h
    add_ff.h
    add_const_bb.h
    add_const_ss.h
    add_const_ii.h
    add_const_ff.h
    add_const_cc.h
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
    complex_to_magphase.h
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
    exponentiate_const_cci.h
    file_descriptor_sink.h
    file_descriptor_source.h
    file_sink.h
    file_source.h
    file_meta_sink.h
    file_meta_source.h
    float_to_char.h
    float_to_complex.h
    magphase_to_complex.h
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
    multiply_const_vcc.h
    multiply_const_ff.h
    multiply_const_vff.h
    multiply_by_tag_value_cc.h
    nlog10_ff.h
    nop.h
    null_sink.h
    null_source.h
    pack_k_bits_bb.h
    patterned_interleaver.h
    pdu.h
    pdu_filter.h
    pdu_set.h
    pdu_remove.h
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
    sub_ff.h
    tag_debug.h
    tag_gate.h
    tag_share.h
    tagged_file_sink.h
    tagged_stream_align.h
    tagged_stream_mux.h
    tagged_stream_multiply_length.h
    tagged_stream_to_pdu.h
    tags_strobe.h
    tcp_server_sink.h
    test_tag_variable_rate_ff.h
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
