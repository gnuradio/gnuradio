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
    filter_dc_blocker_xx.xml
    filter_fft_filter_xxx.xml
    filter_fir_filter_xxx.xml
    filter_filter_delay_fc.xml
    filter_filterbank_vcvcf.xml
    filter_fractional_interpolator_xx.xml
    filter_fractional_resampler_xx.xml
    filter_freq_xlating_fft_filter_ccc.xml
    filter_freq_xlating_fir_filter_xxx.xml
    filter_hilbert_fc.xml
    filter_iir_filter_xxx.xml
    filter_interp_fir_filter_xxx.xml
    filter_pfb_arb_resampler.xml
    filter_pfb_channelizer.xml
    filter_pfb_channelizer_hier.xml
    filter_pfb_decimator.xml
    filter_pfb_interpolator.xml
    filter_pfb_synthesizer.xml
    filter_rational_resampler_xxx.xml
    filter_rational_resampler_base_xxx.xml
    filter_single_pole_iir_filter_xx.xml
    filter_low_pass_filter.xml
    filter_high_pass_filter.xml
    filter_band_pass_filter.xml
    filter_band_reject_filter.xml
    filter_root_raised_cosine_filter.xml
    variable_low_pass_filter_taps.xml
    variable_high_pass_filter_taps.xml
    variable_band_pass_filter_taps.xml
    variable_band_reject_filter_taps.xml
    variable_rrc_filter_taps.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "filter_python"
)
