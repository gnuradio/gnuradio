/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#define ANALOG_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "analog_swig_doc.i"

%{
#include "analog/cpm.h"
#include "analog/noise_type.h"
#include "analog/agc_cc.h"
#include "analog/agc_ff.h"
#include "analog/agc2_cc.h"
#include "analog/agc2_ff.h"
#include "analog/cpfsk_bc.h"
#include "analog/ctcss_squelch_ff.h"
#include "analog/dpll_bb.h"
#include "analog/feedforward_agc_cc.h"
#include "analog/fmdet_cf.h"
#include "analog/frequency_modulator_fc.h"
#include "analog/lfsr.h"
#include "analog/noise_source_s.h"
#include "analog/noise_source_i.h"
#include "analog/noise_source_f.h"
#include "analog/noise_source_c.h"
#include "analog/phase_modulator_fc.h"
#include "analog/pll_carriertracking_cc.h"
#include "analog/pll_freqdet_cf.h"
#include "analog/pll_refout_cc.h"
#include "analog/probe_avg_mag_sqrd_c.h"
#include "analog/probe_avg_mag_sqrd_cf.h"
#include "analog/probe_avg_mag_sqrd_f.h"
#include "analog/pwr_squelch_cc.h"
#include "analog/pwr_squelch_ff.h"
#include "analog/sincos.h"
#include "analog/sig_source_s.h"
#include "analog/sig_source_i.h"
#include "analog/sig_source_f.h"
#include "analog/sig_source_c.h"
#include "analog/sig_source_waveform.h"
#include "analog/simple_squelch_cc.h"
#include "analog/squelch_base_cc.h"
#include "analog/squelch_base_ff.h"
%}

%include "analog/cpm.h"
%include "analog/noise_type.h"
%include "analog/agc_cc.h"
%include "analog/agc_ff.h"
%include "analog/agc2_cc.h"
%include "analog/agc2_ff.h"
%include "analog/cpfsk_bc.h"
%include "analog/ctcss_squelch_ff.h"
%include "analog/dpll_bb.h"
%include "analog/feedforward_agc_cc.h"
%include "analog/fmdet_cf.h"
%include "analog/frequency_modulator_fc.h"
%include "analog/lfsr.h"
%include "analog/noise_source_s.h"
%include "analog/noise_source_i.h"
%include "analog/noise_source_f.h"
%include "analog/noise_source_c.h"
%include "analog/phase_modulator_fc.h"
%include "analog/pll_carriertracking_cc.h"
%include "analog/pll_freqdet_cf.h"
%include "analog/pll_refout_cc.h"
%include "analog/probe_avg_mag_sqrd_c.h"
%include "analog/probe_avg_mag_sqrd_cf.h"
%include "analog/probe_avg_mag_sqrd_f.h"
%include "analog/pwr_squelch_cc.h"
%include "analog/pwr_squelch_ff.h"
%include "analog/sincos.h"
%include "analog/sig_source_s.h"
%include "analog/sig_source_i.h"
%include "analog/sig_source_f.h"
%include "analog/sig_source_c.h"
%include "analog/sig_source_waveform.h"
%include "analog/simple_squelch_cc.h"
%include "analog/squelch_base_cc.h"
%include "analog/squelch_base_ff.h"

GR_SWIG_BLOCK_MAGIC2(analog, agc_cc);
GR_SWIG_BLOCK_MAGIC2(analog, agc_ff);
GR_SWIG_BLOCK_MAGIC2(analog, agc2_cc);
GR_SWIG_BLOCK_MAGIC2(analog, agc2_ff);
GR_SWIG_BLOCK_MAGIC2(analog, cpfsk_bc);
GR_SWIG_BLOCK_MAGIC2(analog, ctcss_squelch_ff);
GR_SWIG_BLOCK_MAGIC2(analog, dpll_bb);
GR_SWIG_BLOCK_MAGIC2(analog, feedforward_agc_cc);
GR_SWIG_BLOCK_MAGIC2(analog, fmdet_cf);
GR_SWIG_BLOCK_MAGIC2(analog, frequency_modulator_fc);
GR_SWIG_BLOCK_MAGIC2(analog, noise_source_s);
GR_SWIG_BLOCK_MAGIC2(analog, noise_source_i);
GR_SWIG_BLOCK_MAGIC2(analog, noise_source_f);
GR_SWIG_BLOCK_MAGIC2(analog, noise_source_c);
GR_SWIG_BLOCK_MAGIC2(analog, phase_modulator_fc);
GR_SWIG_BLOCK_MAGIC2(analog, pll_carriertracking_cc);
GR_SWIG_BLOCK_MAGIC2(analog, pll_freqdet_cf);
GR_SWIG_BLOCK_MAGIC2(analog, pll_refout_cc);
GR_SWIG_BLOCK_MAGIC2(analog, probe_avg_mag_sqrd_c);
GR_SWIG_BLOCK_MAGIC2(analog, probe_avg_mag_sqrd_cf);
GR_SWIG_BLOCK_MAGIC2(analog, probe_avg_mag_sqrd_f);
GR_SWIG_BLOCK_MAGIC2(analog, pwr_squelch_cc);
GR_SWIG_BLOCK_MAGIC2(analog, pwr_squelch_ff);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_s);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_i);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_f);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_c);
GR_SWIG_BLOCK_MAGIC2(analog, simple_squelch_cc);
