
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

void bind_abs_blk(py::module&);
void bind_add_blk(py::module&);
void bind_add_const_bb(py::module&);
void bind_add_const_cc(py::module&);
void bind_add_const_ff(py::module&);
void bind_add_const_ii(py::module&);
void bind_add_const_ss(py::module&);
void bind_add_const_v(py::module&);
void bind_and_blk(py::module&);
void bind_and_const(py::module&);
void bind_annotator_1to1(py::module&);
void bind_annotator_alltoall(py::module&);
void bind_annotator_raw(py::module&);
void bind_argmax(py::module&);
void bind_burst_tagger(py::module&);
void bind_char_to_float(py::module&);
void bind_char_to_short(py::module&);
void bind_check_lfsr_32k_s(py::module&);
void bind_complex_to_arg(py::module&);
void bind_complex_to_float(py::module&);
void bind_complex_to_imag(py::module&);
void bind_complex_to_interleaved_char(py::module&);
void bind_complex_to_interleaved_short(py::module&);
void bind_complex_to_mag(py::module&);
void bind_complex_to_mag_squared(py::module&);
void bind_complex_to_magphase(py::module&);
void bind_complex_to_real(py::module&);
void bind_conjugate_cc(py::module&);
void bind_control_loop(py::module&);
void bind_copy(py::module&);
void bind_correctiq(py::module&);
void bind_correctiq_auto(py::module&);
void bind_correctiq_man(py::module&);
void bind_correctiq_swapiq(py::module&);
void bind_count_bits(py::module&);
// void bind_ctrlport_probe2_b(py::module&);
// void bind_ctrlport_probe2_c(py::module&);
// void bind_ctrlport_probe2_f(py::module&);
// void bind_ctrlport_probe2_i(py::module&);
// void bind_ctrlport_probe2_s(py::module&);
// void bind_ctrlport_probe_c(py::module&);
void bind_deinterleave(py::module&);
void bind_delay(py::module&);
void bind_divide(py::module&);
void bind_endian_swap(py::module&);
void bind_exponentiate_const_cci(py::module&);
void bind_file_descriptor_sink(py::module&);
void bind_file_descriptor_source(py::module&);
void bind_file_meta_sink(py::module&);
void bind_file_meta_source(py::module&);
void bind_file_sink(py::module&);
void bind_file_sink_base(py::module&);
void bind_file_source(py::module&);
void bind_float_to_char(py::module&);
void bind_float_to_complex(py::module&);
void bind_float_to_int(py::module&);
void bind_float_to_short(py::module&);
void bind_float_to_uchar(py::module&);
void bind_head(py::module&);
void bind_int_to_float(py::module&);
void bind_integrate(py::module&);
void bind_interleave(py::module&);
void bind_interleaved_char_to_complex(py::module&);
void bind_interleaved_short_to_complex(py::module&);
void bind_keep_m_in_n(py::module&);
void bind_keep_one_in_n(py::module&);
void bind_lfsr_15_1_0(py::module&);
void bind_lfsr_32k(py::module&);
void bind_lfsr_32k_source_s(py::module&);
void bind_magphase_to_complex(py::module&);
void bind_max_blk(py::module&);
void bind_message_debug(py::module&);
void bind_message_strobe(py::module&);
void bind_message_strobe_random(py::module&);
void bind_min_blk(py::module&);
void bind_moving_average(py::module&);
void bind_multiply(py::module&);
void bind_multiply_by_tag_value_cc(py::module&);
void bind_multiply_conjugate_cc(py::module&);
void bind_multiply_const(py::module&);
void bind_multiply_const_v(py::module&);
void bind_multiply_matrix(py::module&);
void bind_mute(py::module&);
void bind_nlog10_ff(py::module&);
void bind_nop(py::module&);
void bind_not_blk(py::module&);
void bind_null_sink(py::module&);
void bind_null_source(py::module&);
void bind_or_blk(py::module&);
// void bind_pack_k_bits(py::module&);
void bind_pack_k_bits_bb(py::module&);
void bind_packed_to_unpacked(py::module&);
void bind_patterned_interleaver(py::module&);
void bind_peak_detector(py::module&);
void bind_peak_detector2_fb(py::module&);
void bind_phase_shift(py::module&);
void bind_plateau_detector_fb(py::module&);
void bind_probe_rate(py::module&);
void bind_probe_signal(py::module&);
void bind_probe_signal_v(py::module&);
void bind_regenerate_bb(py::module&);
void bind_repack_bits_bb(py::module&);
void bind_repeat(py::module&);
void bind_rms_cf(py::module&);
void bind_rms_ff(py::module&);
void bind_rotator(py::module&);
void bind_rotator_cc(py::module&);
void bind_sample_and_hold(py::module&);
void bind_selector(py::module&);
void bind_short_to_char(py::module&);
void bind_short_to_float(py::module&);
void bind_skiphead(py::module&);
void bind_stream_demux(py::module&);
void bind_stream_mux(py::module&);
void bind_stream_to_streams(py::module&);
void bind_stream_to_tagged_stream(py::module&);
void bind_stream_to_vector(py::module&);
void bind_streams_to_stream(py::module&);
void bind_streams_to_vector(py::module&);
void bind_stretch_ff(py::module&);
void bind_sub(py::module&);
void bind_tag_debug(py::module&);
void bind_tag_gate(py::module&);
void bind_tag_share(py::module&);
void bind_tagged_file_sink(py::module&);
void bind_tagged_stream_align(py::module&);
void bind_tagged_stream_multiply_length(py::module&);
void bind_tagged_stream_mux(py::module&);
void bind_tags_strobe(py::module&);
void bind_tcp_server_sink(py::module&);
void bind_test_tag_variable_rate_ff(py::module&);
void bind_threshold_ff(py::module&);
void bind_throttle(py::module&);
void bind_transcendental(py::module&);
void bind_tsb_vector_sink(py::module&);
void bind_uchar_to_float(py::module&);
void bind_udp_sink(py::module&);
void bind_udp_source(py::module&);
void bind_unpack_k_bits(py::module&);
void bind_unpack_k_bits_bb(py::module&);
void bind_unpacked_to_packed(py::module&);
void bind_vco_c(py::module&);
void bind_vco_f(py::module&);
void bind_vector_insert(py::module&);
void bind_vector_map(py::module&);
void bind_vector_sink(py::module&);
void bind_vector_source(py::module&);
void bind_vector_to_stream(py::module&);
void bind_vector_to_streams(py::module&);
#ifndef NO_WAVFILE
void bind_wavfile(py::module&);
void bind_wavfile_sink(py::module&);
void bind_wavfile_source(py::module&);
#endif
void bind_xor_blk(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(blocks_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_abs_blk(m);
    bind_add_blk(m);
    bind_add_const_bb(m);
    bind_add_const_cc(m);
    bind_add_const_ff(m);
    bind_add_const_ii(m);
    bind_add_const_ss(m);
    bind_add_const_v(m);
    bind_and_blk(m);
    bind_and_const(m);
    bind_annotator_1to1(m);
    bind_annotator_alltoall(m);
    bind_annotator_raw(m);
    bind_argmax(m);
    bind_burst_tagger(m);
    bind_char_to_float(m);
    bind_char_to_short(m);
    bind_check_lfsr_32k_s(m);
    bind_complex_to_arg(m);
    bind_complex_to_float(m);
    bind_complex_to_imag(m);
    bind_complex_to_interleaved_char(m);
    bind_complex_to_interleaved_short(m);
    bind_complex_to_mag(m);
    bind_complex_to_mag_squared(m);
    bind_complex_to_magphase(m);
    bind_complex_to_real(m);
    bind_conjugate_cc(m);
    bind_control_loop(m);
    bind_copy(m);
    bind_correctiq(m);
    bind_correctiq_auto(m);
    bind_correctiq_man(m);
    bind_correctiq_swapiq(m);
    bind_count_bits(m);
    // bind_ctrlport_probe2_b(m);
    // bind_ctrlport_probe2_c(m);
    // bind_ctrlport_probe2_f(m);
    // bind_ctrlport_probe2_i(m);
    // bind_ctrlport_probe2_s(m);
    // bind_ctrlport_probe_c(m);
    bind_deinterleave(m);
    bind_delay(m);
    bind_divide(m);
    bind_endian_swap(m);
    bind_exponentiate_const_cci(m);
    bind_file_descriptor_sink(m);
    bind_file_descriptor_source(m);
    bind_file_meta_sink(m);
    bind_file_meta_source(m);
    bind_file_sink_base(m);
    bind_file_sink(m);
    bind_file_source(m);
    bind_float_to_char(m);
    bind_float_to_complex(m);
    bind_float_to_int(m);
    bind_float_to_short(m);
    bind_float_to_uchar(m);
    bind_head(m);
    bind_int_to_float(m);
    bind_integrate(m);
    bind_interleave(m);
    bind_interleaved_char_to_complex(m);
    bind_interleaved_short_to_complex(m);
    bind_keep_m_in_n(m);
    bind_keep_one_in_n(m);
    bind_lfsr_15_1_0(m);
    bind_lfsr_32k(m);
    bind_lfsr_32k_source_s(m);
    bind_magphase_to_complex(m);
    bind_max_blk(m);
    bind_message_debug(m);
    bind_message_strobe(m);
    bind_message_strobe_random(m);
    bind_min_blk(m);
    bind_moving_average(m);
    bind_multiply(m);
    bind_multiply_by_tag_value_cc(m);
    bind_multiply_conjugate_cc(m);
    bind_multiply_const(m);
    bind_multiply_const_v(m);
    bind_multiply_matrix(m);
    bind_mute(m);
    bind_nlog10_ff(m);
    bind_nop(m);
    bind_not_blk(m);
    bind_null_sink(m);
    bind_null_source(m);
    bind_or_blk(m);
    // bind_pack_k_bits(m);
    bind_pack_k_bits_bb(m);
    bind_packed_to_unpacked(m);
    bind_patterned_interleaver(m);
    bind_peak_detector(m);
    bind_peak_detector2_fb(m);
    bind_plateau_detector_fb(m);
    bind_probe_rate(m);
    bind_probe_signal(m);
    bind_probe_signal_v(m);
    bind_regenerate_bb(m);
    bind_repack_bits_bb(m);
    bind_repeat(m);
    bind_rms_cf(m);
    bind_rms_ff(m);
    bind_rotator(m);
    bind_rotator_cc(m);
    bind_sample_and_hold(m);
    bind_selector(m);
    bind_short_to_char(m);
    bind_short_to_float(m);
    bind_skiphead(m);
    bind_stream_demux(m);
    bind_stream_mux(m);
    bind_stream_to_streams(m);
    bind_stream_to_tagged_stream(m);
    bind_stream_to_vector(m);
    bind_streams_to_stream(m);
    bind_streams_to_vector(m);
    bind_stretch_ff(m);
    bind_sub(m);
    bind_tag_debug(m);
    bind_tag_gate(m);
    bind_tag_share(m);
    bind_tagged_file_sink(m);
    bind_tagged_stream_align(m);
    bind_tagged_stream_multiply_length(m);
    bind_tagged_stream_mux(m);
    bind_tags_strobe(m);
    bind_tcp_server_sink(m);
    bind_test_tag_variable_rate_ff(m);
    bind_threshold_ff(m);
    bind_throttle(m);
    bind_transcendental(m);
    bind_tsb_vector_sink(m);
    bind_uchar_to_float(m);
    bind_udp_sink(m);
    bind_udp_source(m);
    bind_unpack_k_bits(m);
    bind_unpack_k_bits_bb(m);
    bind_unpacked_to_packed(m);
    bind_vco_c(m);
    bind_vco_f(m);
    bind_vector_insert(m);
    bind_vector_map(m);
    bind_vector_sink(m);
    bind_vector_source(m);
    bind_vector_to_stream(m);
    bind_vector_to_streams(m);
#ifndef NO_WAVFILE
    bind_wavfile(m);
    bind_wavfile_sink(m);
    bind_wavfile_source(m);
#endif
    bind_xor_blk(m);
}
