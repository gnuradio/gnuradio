
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

void bind_adaptive_algorithm(py::module&);
void bind_adaptive_algorithm_cma(py::module&);
void bind_adaptive_algorithm_lms(py::module&);
void bind_adaptive_algorithm_nlms(py::module&);
void bind_additive_scrambler_bb(py::module&);
void bind_binary_slicer_fb(py::module&);
void bind_burst_shaper(py::module&);
void bind_chunks_to_symbols(py::module&);
void bind_clock_recovery_mm_cc(py::module&);
void bind_clock_recovery_mm_ff(py::module&);
void bind_constellation(py::module&);
void bind_constellation_decoder_cb(py::module&);
void bind_constellation_encoder_bc(py::module&);
void bind_constellation_receiver_cb(py::module&);
void bind_constellation_soft_decoder_cf(py::module&);
void bind_corr_est_cc(py::module&);
void bind_correlate_access_code_bb(py::module&);
void bind_correlate_access_code_bb_ts(py::module&);
void bind_correlate_access_code_ff_ts(py::module&);
void bind_correlate_access_code_tag_bb(py::module&);
void bind_correlate_access_code_tag_ff(py::module&);
void bind_costas_loop_cc(py::module&);
void bind_cpmmod_bc(py::module&);
void bind_crc32(py::module&);
void bind_crc32_async_bb(py::module&);
void bind_crc32_bb(py::module&);
void bind_decision_feedback_equalizer(py::module&);
void bind_descrambler_bb(py::module&);
void bind_diff_decoder_bb(py::module&);
void bind_diff_encoder_bb(py::module&);
void bind_diff_phasor_cc(py::module&);
void bind_fll_band_edge_cc(py::module&);
void bind_framer_sink_1(py::module&);
void bind_glfsr(py::module&);
void bind_glfsr_source_b(py::module&);
void bind_glfsr_source_f(py::module&);
void bind_hdlc_deframer_bp(py::module&);
void bind_hdlc_framer_pb(py::module&);
void bind_header_buffer(py::module&);
void bind_header_format_base(py::module&);
void bind_header_format_counter(py::module&);
void bind_header_format_crc(py::module&);
void bind_header_format_default(py::module&);
void bind_header_format_ofdm(py::module&);
void bind_header_payload_demux(py::module&);
void bind_interpolating_resampler_type(py::module&);
void bind_lfsr(py::module&);
void bind_linear_equalizer(py::module&);
void bind_map_bb(py::module&);
void bind_meas_evm_cc(py::module&);
void bind_metric_type(py::module&);
void bind_modulate_vector(py::module&);
void bind_mpsk_snr_est(py::module&);
void bind_mpsk_snr_est_cc(py::module&);
void bind_msk_timing_recovery_cc(py::module&);
void bind_ofdm_carrier_allocator_cvc(py::module&);
void bind_ofdm_chanest_vcvc(py::module&);
void bind_ofdm_cyclic_prefixer(py::module&);
void bind_ofdm_equalizer_base(py::module&);
void bind_ofdm_equalizer_simpledfe(py::module&);
void bind_ofdm_equalizer_static(py::module&);
void bind_ofdm_frame_equalizer_vcvc(py::module&);
void bind_ofdm_serializer_vcc(py::module&);
void bind_ofdm_sync_sc_cfb(py::module&);
void bind_packet_header_default(py::module&);
void bind_packet_header_ofdm(py::module&);
void bind_packet_headergenerator_bb(py::module&);
void bind_packet_headerparser_b(py::module&);
void bind_packet_sink(py::module&);
void bind_pfb_clock_sync_ccf(py::module&);
void bind_pfb_clock_sync_fff(py::module&);
void bind_pn_correlator_cc(py::module&);
void bind_probe_density_b(py::module&);
void bind_probe_mpsk_snr_est_c(py::module&);
void bind_protocol_formatter_async(py::module&);
void bind_protocol_formatter_bb(py::module&);
void bind_protocol_parser_b(py::module&);
void bind_scrambler_bb(py::module&);
void bind_symbol_sync_cc(py::module&);
void bind_symbol_sync_ff(py::module&);
void bind_timing_error_detector_type(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(digital_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");
    py::module::import("gnuradio.blocks");

    bind_adaptive_algorithm(m);
    bind_adaptive_algorithm_cma(m);
    bind_adaptive_algorithm_lms(m);
    bind_adaptive_algorithm_nlms(m);
    bind_additive_scrambler_bb(m);
    bind_binary_slicer_fb(m);
    bind_burst_shaper(m);
    bind_chunks_to_symbols(m);
    bind_clock_recovery_mm_cc(m);
    bind_clock_recovery_mm_ff(m);
    bind_constellation(m);
    bind_constellation_decoder_cb(m);
    bind_constellation_encoder_bc(m);
    bind_constellation_receiver_cb(m);
    bind_constellation_soft_decoder_cf(m);
    bind_corr_est_cc(m);
    bind_correlate_access_code_bb(m);
    bind_correlate_access_code_bb_ts(m);
    bind_correlate_access_code_ff_ts(m);
    bind_correlate_access_code_tag_bb(m);
    bind_correlate_access_code_tag_ff(m);
    bind_costas_loop_cc(m);
    bind_cpmmod_bc(m);
    bind_crc32(m);
    bind_crc32_async_bb(m);
    bind_crc32_bb(m);
    bind_decision_feedback_equalizer(m);
    bind_descrambler_bb(m);
    bind_diff_decoder_bb(m);
    bind_diff_encoder_bb(m);
    bind_diff_phasor_cc(m);
    bind_fll_band_edge_cc(m);
    bind_framer_sink_1(m);
    bind_glfsr(m);
    bind_glfsr_source_b(m);
    bind_glfsr_source_f(m);
    bind_hdlc_deframer_bp(m);
    bind_hdlc_framer_pb(m);
    bind_header_buffer(m);
    bind_header_format_base(m);
    bind_header_format_default(m);
    bind_header_format_counter(m);
    bind_header_format_crc(m);
    bind_header_format_ofdm(m);
    bind_header_payload_demux(m);
    bind_interpolating_resampler_type(m);
    bind_lfsr(m);
    bind_linear_equalizer(m);
    bind_map_bb(m);
    bind_meas_evm_cc(m);
    bind_metric_type(m);
    bind_modulate_vector(m);
    bind_mpsk_snr_est(m);
    bind_mpsk_snr_est_cc(m);
    bind_msk_timing_recovery_cc(m);
    bind_ofdm_carrier_allocator_cvc(m);
    bind_ofdm_chanest_vcvc(m);
    bind_ofdm_cyclic_prefixer(m);
    bind_ofdm_equalizer_base(m);
    bind_ofdm_equalizer_simpledfe(m);
    bind_ofdm_equalizer_static(m);
    bind_ofdm_frame_equalizer_vcvc(m);
    bind_ofdm_serializer_vcc(m);
    bind_ofdm_sync_sc_cfb(m);
    bind_packet_header_default(m);
    bind_packet_header_ofdm(m);
    bind_packet_headergenerator_bb(m);
    bind_packet_headerparser_b(m);
    bind_packet_sink(m);
    bind_pfb_clock_sync_ccf(m);
    bind_pfb_clock_sync_fff(m);
    bind_pn_correlator_cc(m);
    bind_probe_density_b(m);
    bind_probe_mpsk_snr_est_c(m);
    bind_protocol_formatter_async(m);
    bind_protocol_formatter_bb(m);
    bind_protocol_parser_b(m);
    bind_scrambler_bb(m);
    bind_symbol_sync_cc(m);
    bind_symbol_sync_ff(m);
    bind_timing_error_detector_type(m);
}
