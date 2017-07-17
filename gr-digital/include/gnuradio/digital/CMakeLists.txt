# Copyright (C) 2011-2017 Free Software Foundation, Inc.
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
#######################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(digital chunks_to_symbols_XX     bf bc sf sc if ic)
GR_EXPAND_X_H(digital burst_shaper_XX     cc ff)

add_custom_target(digital_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    additive_scrambler_bb.h
    api.h
    binary_slicer_fb.h
    clock_recovery_mm_cc.h
    clock_recovery_mm_ff.h
    cma_equalizer_cc.h
    constellation.h
    constellation_decoder_cb.h
    constellation_receiver_cb.h
    constellation_soft_decoder_cf.h
    corr_est_cc.h
    correlate_access_code_bb.h
    correlate_access_code_tag_bb.h
    correlate_access_code_tag_ff.h
    correlate_access_code_bb_ts.h
    correlate_access_code_ff_ts.h
    correlate_and_sync_cc.h
    costas_loop_cc.h
    cpmmod_bc.h
    crc32.h
    crc32_bb.h
    crc32_async_bb.h
    descrambler_bb.h
    diff_decoder_bb.h
    diff_encoder_bb.h
    diff_phasor_cc.h
    fll_band_edge_cc.h
    framer_sink_1.h
    glfsr.h
    glfsr_source_b.h
    glfsr_source_f.h
    hdlc_deframer_bp.h
    hdlc_framer_pb.h
    header_buffer.h
    header_payload_demux.h
    kurtotic_equalizer_cc.h
    lfsr.h
    lms_dd_equalizer_cc.h
    map_bb.h
    metric_type.h
    modulate_vector.h
    mpsk_receiver_cc.h
    mpsk_snr_est.h
    mpsk_snr_est_cc.h
    msk_timing_recovery_cc.h
    ofdm_carrier_allocator_cvc.h
    ofdm_chanest_vcvc.h
    ofdm_cyclic_prefixer.h
    ofdm_equalizer_base.h
    ofdm_equalizer_simpledfe.h
    ofdm_equalizer_static.h
    ofdm_frame_acquisition.h
    ofdm_frame_equalizer_vcvc.h
    ofdm_frame_sink.h
    ofdm_insert_preamble.h
    ofdm_mapper_bcv.h
    ofdm_sampler.h
    ofdm_serializer_vcc.h
    ofdm_sync_sc_cfb.h
    header_format_base.h
    header_format_default.h
    header_format_counter.h
    header_format_crc.h
    header_format_ofdm.h
    protocol_formatter_async.h
    protocol_formatter_bb.h
    protocol_parser_b.h
    packet_header_default.h
    packet_header_ofdm.h
    packet_headergenerator_bb.h
    packet_headerparser_b.h
    packet_sink.h
    pfb_clock_sync_ccf.h
    pfb_clock_sync_fff.h
    pn_correlator_cc.h
    probe_density_b.h
    probe_mpsk_snr_est_c.h
    scrambler_bb.h
    simple_correlator.h
    simple_framer.h
    simple_framer_sync.h
    interpolating_resampler_type.h
    symbol_sync_cc.h
    symbol_sync_ff.h
    timing_error_detector_type.h
    header_payload_demux.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/digital
    COMPONENT "digital_devel"
)
