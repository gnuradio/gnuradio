# Copyright 2014,2015 Free Software Foundation, Inc.
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
# Install header files
########################################################################
install(FILES
    api.h
    atsc_consts.h
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
    dvbs2_config.h
    dvbt2_config.h
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
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/dtv
    COMPONENT "dtv_devel"
)
