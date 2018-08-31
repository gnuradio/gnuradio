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
    filter.tree.yml
    filter_dc_blocker_xx.block.yml
    filter_fft_filter_xxx.block.yml
    filter_fir_filter_xxx.block.yml
    filter_filter_delay_fc.block.yml
    filter_filterbank_vcvcf.block.yml
    filter_mmse_interpolator_xx.block.yml
    filter_mmse_resampler_xx.block.yml
    filter_freq_xlating_fft_filter_ccc.block.yml
    filter_freq_xlating_fir_filter_xxx.block.yml
    filter_hilbert_fc.block.yml
    filter_iir_filter_xxx.block.yml
    filter_interp_fir_filter_xxx.block.yml
    filter_pfb_arb_resampler.block.yml
    filter_pfb_channelizer.block.yml
    filter_pfb_channelizer_hier.block.yml
    filter_pfb_decimator.block.yml
    filter_pfb_interpolator.block.yml
    filter_pfb_synthesizer.block.yml
    filter_rational_resampler_xxx.block.yml
    filter_rational_resampler_base_xxx.block.yml
    filter_single_pole_iir_filter_xx.block.yml
    filter_low_pass_filter.block.yml
    filter_high_pass_filter.block.yml
    filter_band_pass_filter.block.yml
    filter_band_reject_filter.block.yml
    filter_root_raised_cosine_filter.block.yml
    variable_low_pass_filter_taps.block.yml
    variable_high_pass_filter_taps.block.yml
    variable_band_pass_filter_taps.block.yml
    variable_band_reject_filter_taps.block.yml
    variable_rrc_filter_taps.block.yml
    DESTINATION ${GRC_BLOCKS_DIR}
)
