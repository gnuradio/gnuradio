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

install(FILES
    dtv_atsc_deinterleaver.xml
    dtv_atsc_depad.xml
    dtv_atsc_derandomizer.xml
    dtv_atsc_equalizer.xml
    dtv_atsc_fpll.xml
    dtv_atsc_fs_checker.xml
    dtv_atsc_rs_decoder.xml
    dtv_atsc_rx.xml
    dtv_atsc_rx_filter.xml
    dtv_atsc_sync.xml
    dtv_atsc_viterbi_decoder.xml
    dtv_block_tree.xml
    dtv_atsc_pad.xml
    dtv_atsc_randomizer.xml
    dtv_atsc_rs_encoder.xml
    dtv_atsc_interleaver.xml
    dtv_atsc_trellis_encoder.xml
    dtv_atsc_field_sync_mux.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "dtv_python"
)
