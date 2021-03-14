# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND blocks_python_files
    abs_blk_python.cc
    add_blk_python.cc
    add_const_bb_python.cc
    add_const_cc_python.cc
    add_const_ff_python.cc
    add_const_ii_python.cc
    add_const_ss_python.cc
    add_const_v_python.cc
    and_blk_python.cc
    and_const_python.cc
    annotator_1to1_python.cc
    annotator_alltoall_python.cc
    annotator_raw_python.cc
    argmax_python.cc
    burst_tagger_python.cc
    char_to_float_python.cc
    char_to_short_python.cc
    check_lfsr_32k_s_python.cc
    complex_to_arg_python.cc
    complex_to_float_python.cc
    complex_to_imag_python.cc
    complex_to_interleaved_char_python.cc
    complex_to_interleaved_short_python.cc
    complex_to_mag_python.cc
    complex_to_mag_squared_python.cc
    complex_to_magphase_python.cc
    complex_to_real_python.cc
    conjugate_cc_python.cc
    control_loop_python.cc
    copy_python.cc
    correctiq_python.cc
    correctiq_auto_python.cc
    correctiq_man_python.cc
    correctiq_swapiq_python.cc
    count_bits_python.cc
    # ctrlport_probe2_b_python.cc
    # ctrlport_probe2_c_python.cc
    # ctrlport_probe2_f_python.cc
    # ctrlport_probe2_i_python.cc
    # ctrlport_probe2_s_python.cc
    # ctrlport_probe_c_python.cc
    deinterleave_python.cc
    delay_python.cc
    divide_python.cc
    endian_swap_python.cc
    exponentiate_const_cci_python.cc
    file_descriptor_sink_python.cc
    file_descriptor_source_python.cc
    file_meta_sink_python.cc
    file_meta_source_python.cc
    file_sink_python.cc
    file_sink_base_python.cc
    file_source_python.cc
    float_to_char_python.cc
    float_to_complex_python.cc
    float_to_int_python.cc
    float_to_short_python.cc
    float_to_uchar_python.cc
    head_python.cc
    int_to_float_python.cc
    integrate_python.cc
    interleave_python.cc
    interleaved_char_to_complex_python.cc
    interleaved_short_to_complex_python.cc
    keep_m_in_n_python.cc
    keep_one_in_n_python.cc
    lfsr_15_1_0_python.cc
    lfsr_32k_python.cc
    lfsr_32k_source_s_python.cc
    magphase_to_complex_python.cc
    max_blk_python.cc
    message_debug_python.cc
    message_strobe_python.cc
    message_strobe_random_python.cc
    min_blk_python.cc
    moving_average_python.cc
    multiply_python.cc
    multiply_by_tag_value_cc_python.cc
    multiply_conjugate_cc_python.cc
    multiply_const_python.cc
    multiply_const_v_python.cc
    multiply_matrix_python.cc
    mute_python.cc
    nlog10_ff_python.cc
    nop_python.cc
    not_blk_python.cc
    null_sink_python.cc
    null_source_python.cc
    or_blk_python.cc
    # pack_k_bits_python.cc
    pack_k_bits_bb_python.cc
    packed_to_unpacked_python.cc
    patterned_interleaver_python.cc
    peak_detector_python.cc
    peak_detector2_fb_python.cc
    phase_shift_python.cc
    plateau_detector_fb_python.cc
    probe_rate_python.cc
    probe_signal_python.cc
    probe_signal_v_python.cc
    regenerate_bb_python.cc
    repack_bits_bb_python.cc
    repeat_python.cc
    rms_cf_python.cc
    rms_ff_python.cc
    rotator_python.cc
    rotator_cc_python.cc
    sample_and_hold_python.cc
    selector_python.cc
    short_to_char_python.cc
    short_to_float_python.cc
    skiphead_python.cc
    stream_demux_python.cc
    stream_mux_python.cc
    stream_to_streams_python.cc
    stream_to_tagged_stream_python.cc
    stream_to_vector_python.cc
    streams_to_stream_python.cc
    streams_to_vector_python.cc
    stretch_ff_python.cc
    sub_python.cc
    tag_debug_python.cc
    tag_gate_python.cc
    tag_share_python.cc
    tagged_file_sink_python.cc
    tagged_stream_align_python.cc
    tagged_stream_multiply_length_python.cc
    tagged_stream_mux_python.cc
    tags_strobe_python.cc
    tcp_server_sink_python.cc
    test_tag_variable_rate_ff_python.cc
    threshold_ff_python.cc
    throttle_python.cc
    transcendental_python.cc
    tsb_vector_sink_python.cc
    uchar_to_float_python.cc
    udp_sink_python.cc
    udp_source_python.cc
    unpack_k_bits_python.cc
    unpack_k_bits_bb_python.cc
    unpacked_to_packed_python.cc
    vco_c_python.cc
    vco_f_python.cc
    vector_insert_python.cc
    vector_map_python.cc
    vector_sink_python.cc
    vector_source_python.cc
    vector_to_stream_python.cc
    vector_to_streams_python.cc
    xor_blk_python.cc
    python_bindings.cc)

if (SNDFILE_FOUND)
    list(APPEND blocks_python_files
        wavfile_python.cc
        wavfile_sink_python.cc
        wavfile_source_python.cc)
endif()

GR_PYBIND_MAKE_CHECK_HASH(blocks
  ../../..
  gr::blocks
  "${blocks_python_files}")

install(TARGETS blocks_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks COMPONENT pythonapi)
