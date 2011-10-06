/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2006,2007,2009 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

%{
#include <gr_iir_filter_ffd.h>
#include <gr_single_pole_iir_filter_ff.h>
#include <gr_single_pole_iir_filter_cc.h>
#include <gr_hilbert_fc.h>
#include <gr_filter_delay_fc.h>
#include <gr_fft_filter_ccc.h>
#include <gr_fft_filter_fff.h>
#include <gr_fractional_interpolator_ff.h>
#include <gr_fractional_interpolator_cc.h>
#include <gr_goertzel_fc.h>
#include <gr_pfb_channelizer_ccf.h>
#include <gr_pfb_synthesis_filterbank_ccf.h>
#include <gr_pfb_decimator_ccf.h>
#include <gr_pfb_interpolator_ccf.h>
#include <gr_pfb_arb_resampler_ccf.h>
#include <gr_pfb_arb_resampler_fff.h>
#include <gr_pfb_clock_sync_ccf.h>
#include <gr_pfb_clock_sync_fff.h>
#include <gr_dc_blocker_cc.h>
#include <gr_dc_blocker_ff.h>
%}

%include "gr_iir_filter_ffd.i"
%include "gr_single_pole_iir_filter_ff.i"
%include "gr_single_pole_iir_filter_cc.i"
%include "gr_hilbert_fc.i"
%include "gr_filter_delay_fc.i"
%include "gr_fft_filter_ccc.i"
%include "gr_fft_filter_fff.i"
%include "gr_fractional_interpolator_ff.i"
%include "gr_fractional_interpolator_cc.i"
%include "gr_goertzel_fc.i"
%include "gr_pfb_channelizer_ccf.i"
%include "gr_pfb_synthesis_filterbank_ccf.i"
%include "gr_pfb_decimator_ccf.i"
%include "gr_pfb_interpolator_ccf.i"
%include "gr_pfb_arb_resampler_ccf.i"
%include "gr_pfb_arb_resampler_fff.i"
%include "gr_pfb_decimator_ccf.i"
%include "gr_pfb_interpolator_ccf.i"
%include "gr_pfb_arb_resampler_ccf.i"
%include "gr_pfb_clock_sync_ccf.i"
%include "gr_pfb_clock_sync_fff.i"
%include "gr_dc_blocker_cc.i"
%include "gr_dc_blocker_ff.i"

%include "filter_generated.i"
