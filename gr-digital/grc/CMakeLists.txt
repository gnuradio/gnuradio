# Copyright 2011 Free Software Foundation, Inc.
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
    digital_block_tree.xml
    digital_binary_slicer_fb.xml
    digital_clock_recovery_mm_xx.xml
    digital_constellation_decoder_cb.xml
    digital_correlate_access_code_bb.xml
    digital_costas_loop_cc.xml
    digital_cma_equalizer_cc.xml
    digital_fll_band_edge_cc.xml
    digital_kurtotic_equalizer_cc.xml
    digital_lms_dd_equalizer_cc.xml
    digital_mpsk_receiver_cc.xml
    digital_dxpsk_mod.xml
    digital_dxpsk_demod.xml
    digital_psk_mod.xml
    digital_psk_demod.xml
    digital_qam_mod.xml
    digital_qam_demod.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "digital_python"
)
