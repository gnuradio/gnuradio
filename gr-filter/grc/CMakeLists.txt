# Copyright 2012 Free Software Foundation, Inc.
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
    filter_block_tree.xml
    dc_blocker_xx.xml
    fft_filter_xxx.xml
    fir_filter_xxx.xml
    filter_delay_fc.xml
    fractional_interpolator_xx.xml
    freq_xlating_fir_filter_xxx.xml
    hilbert_fc.xml
    iir_filter_ffd.xml
    interp_fir_filter_xxx.xml
    pfb_arb_resampler.xml
    pfb_channelizer.xml
    pfb_decimator.xml
    pfb_interpolator.xml
    pfb_synthesizer.xml
    rational_resampler_base_xxx.xml
    single_pole_iir_filter_xx.xml
    channel_model.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "filter_python"
)
