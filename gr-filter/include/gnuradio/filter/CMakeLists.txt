# Copyright (C) 2012,2014,2017,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(
    FILES api.h
          firdes.h
          fir_filter.h
          fir_filter_blk.h
          fir_filter_with_buffer.h
          fft_filter.h
          ival_decimator.h
          iir_filter.h
          interpolator_taps.h
          interp_fir_filter.h
          mmse_fir_interpolator.h
          mmse_fir_interpolator_cc.h
          mmse_fir_interpolator_ff.h
          mmse_interp_differentiator_cc.h
          mmse_interp_differentiator_ff.h
          pm_remez.h
          polyphase_filterbank.h
          filterbank.h
          filterbank_vcvcf.h
          single_pole_iir.h
          dc_blocker_cc.h
          dc_blocker_ff.h
          filter_delay_fc.h
          fft_filter_ccc.h
          fft_filter_ccf.h
          fft_filter_fff.h
          freq_xlating_fir_filter.h
          mmse_resampler.h
          mmse_resampler_cc.h # legacy for templated mmse_resampler<sample_t>
          mmse_resampler_ff.h # legacy for templated mmse_resampler<sample_t>
          hilbert_fc.h
          iir_filter_ffd.h
          iir_filter_ccc.h
          iir_filter_ccf.h
          iir_filter_ccd.h
          iir_filter_ccz.h
          pfb_arb_resampler.h
          pfb_arb_resampler_ccf.h
          pfb_arb_resampler_ccc.h
          pfb_arb_resampler_fff.h
          pfb_channelizer_ccf.h
          pfb_decimator_ccf.h
          pfb_interpolator_ccf.h
          pfb_synthesizer_ccf.h
          rational_resampler.h
          single_pole_iir_filter_cc.h
          single_pole_iir_filter_ff.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/filter)
