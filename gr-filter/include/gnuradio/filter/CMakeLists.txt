# Copyright (C) 2012,2014,2017 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
#######################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(filter fir_filter_XXX              ccc ccf fcc fff fsf scc)
GR_EXPAND_X_H(filter freq_xlating_fir_filter_XXX ccc ccf fcc fcf scf scc)
GR_EXPAND_X_H(filter interp_fir_filter_XXX       ccc ccf fcc fff fsf scc)
GR_EXPAND_X_H(filter rational_resampler_base_XXX ccc ccf fcc fff fsf scc)

add_custom_target(filter_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    firdes.h
    fir_filter.h
    fir_filter_with_buffer.h
    fft_filter.h
    iir_filter.h
    interpolator_taps.h
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
    fractional_interpolator_cc.h
    fractional_interpolator_ff.h
    fractional_resampler_cc.h
    fractional_resampler_ff.h
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
    single_pole_iir_filter_cc.h
    single_pole_iir_filter_ff.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/filter
    COMPONENT "filter_devel"
)

