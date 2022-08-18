# Copyright 2012,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES filter.tree.yml
          filter_dc_blocker_xx.block.yml
          filter_fft_filter_xxx.block.yml
          filter_fft_low_pass_filter.block.yml
          filter_fft_root_raised_cosine_filter.block.yml
          filter_fir_filter_xxx.block.yml
          filter_filter_delay_fc.block.yml
          filter_filterbank_vcvcf.block.yml
          filter_ival_decimator.block.yml
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
          variable_file_filter_taps.block.yml
          variable_rrc_filter_taps.block.yml
    DESTINATION ${GRC_BLOCKS_DIR})
