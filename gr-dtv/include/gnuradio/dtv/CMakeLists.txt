# Copyright 2014,2015,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
    atsc_consts.h
    atsc_plinfo.h
    atsc_deinterleaver.h
    atsc_depad.h
    atsc_derandomizer.h
    atsc_equalizer.h
    atsc_fpll.h
    atsc_fs_checker.h
    atsc_rs_decoder.h
    atsc_sync.h
    atsc_viterbi_decoder.h
    atsc_pad.h
    atsc_randomizer.h
    atsc_rs_encoder.h
    atsc_interleaver.h
    atsc_trellis_encoder.h
    atsc_field_sync_mux.h
    dvb_config.h
    dvbt_config.h
    dvbs2_config.h
    dvbt2_config.h
    catv_config.h
    dvb_bbheader_bb.h
    dvb_bbscrambler_bb.h
    dvb_bch_bb.h
    dvb_ldpc_bb.h
    dvbt2_interleaver_bb.h
    dvbt2_modulator_bc.h
    dvbt2_cellinterleaver_cc.h
    dvbt2_framemapper_cc.h
    dvbt2_freqinterleaver_cc.h
    dvbt2_pilotgenerator_cc.h
    dvbt2_paprtr_cc.h
    dvbt2_p1insertion_cc.h
    dvbt2_miso_cc.h
    dvbs2_interleaver_bb.h
    dvbs2_modulator_bc.h
    dvbs2_physical_cc.h
    dvbt_energy_dispersal.h
    dvbt_reed_solomon_enc.h
    dvbt_convolutional_interleaver.h
    dvbt_inner_coder.h
    dvbt_bit_inner_interleaver.h
    dvbt_symbol_inner_interleaver.h
    dvbt_map.h
    dvbt_reference_signals.h
    dvbt_ofdm_sym_acquisition.h
    dvbt_demod_reference_signals.h
    dvbt_demap.h
    dvbt_bit_inner_deinterleaver.h
    dvbt_viterbi_decoder.h
    dvbt_convolutional_deinterleaver.h
    dvbt_reed_solomon_dec.h
    dvbt_energy_descramble.h
    catv_transport_framing_enc_bb.h
    catv_reed_solomon_enc_bb.h
    catv_randomizer_bb.h
    catv_frame_sync_enc_bb.h
    catv_trellis_enc_bb.h

    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/dtv
)
