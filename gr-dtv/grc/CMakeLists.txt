# Copyright 2014,2015,2016 Free Software Foundation, Inc.
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

install(FILES
    dtv_atsc_deinterleaver.block.yml
    dtv_atsc_depad.block.yml
    dtv_atsc_derandomizer.block.yml
    dtv_atsc_equalizer.block.yml
    dtv_atsc_fpll.block.yml
    dtv_atsc_fs_checker.block.yml
    dtv_atsc_rs_decoder.block.yml
    dtv_atsc_rx.block.yml
    dtv_atsc_rx_filter.block.yml
    dtv_atsc_sync.block.yml
    dtv_atsc_viterbi_decoder.block.yml
    dtv_atsc_pad.block.yml
    dtv_atsc_randomizer.block.yml
    dtv_atsc_rs_encoder.block.yml
    dtv_atsc_interleaver.block.yml
    dtv_atsc_trellis_encoder.block.yml
    dtv_atsc_field_sync_mux.block.yml
    dtv_dvb_bbheader_bb.block.yml
    dtv_dvb_bbscrambler_bb.block.yml
    dtv_dvb_bch_bb.block.yml
    dtv_dvb_ldpc_bb.block.yml
    dtv_dvbt2_interleaver_bb.block.yml
    dtv_dvbt2_modulator_bc.block.yml
    dtv_dvbt2_cellinterleaver_cc.block.yml
    dtv_dvbt2_framemapper_cc.block.yml
    dtv_dvbt2_freqinterleaver_cc.block.yml
    dtv_dvbt2_pilotgenerator_cc.block.yml
    dtv_dvbt2_paprtr_cc.block.yml
    dtv_dvbt2_p1insertion_cc.block.yml
    dtv_dvbt2_miso_cc.block.yml
    dtv_dvbs2_interleaver_bb.block.yml
    dtv_dvbs2_modulator_bc.block.yml
    dtv_dvbs2_physical_cc.block.yml
    dtv_dvbt_energy_dispersal.block.yml
    dtv_dvbt_reed_solomon_enc.block.yml
    dtv_dvbt_convolutional_interleaver.block.yml
    dtv_dvbt_inner_coder.block.yml
    dtv_dvbt_bit_inner_interleaver.block.yml
    dtv_dvbt_symbol_inner_interleaver.block.yml
    dtv_dvbt_map.block.yml
    dtv_dvbt_reference_signals.block.yml
    dtv_dvbt_ofdm_sym_acquisition.block.yml
    dtv_dvbt_demod_reference_signals.block.yml
    dtv_dvbt_demap.block.yml
    dtv_dvbt_bit_inner_deinterleaver.block.yml
    dtv_dvbt_viterbi_decoder.block.yml
    dtv_dvbt_convolutional_deinterleaver.block.yml
    dtv_dvbt_reed_solomon_dec.block.yml
    dtv_dvbt_energy_descramble.block.yml
    dtv_catv_transport_framing_enc_bb.block.yml
    dtv_catv_reed_solomon_enc_bb.block.yml
    dtv_catv_randomizer_bb.block.yml
    dtv_catv_frame_sync_enc_bb.block.yml
    dtv_catv_trellis_enc_bb.block.yml
    dtv.tree.yml
    DESTINATION ${GRC_BLOCKS_DIR}
)
