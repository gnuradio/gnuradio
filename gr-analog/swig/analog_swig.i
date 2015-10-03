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
#define BLOCKS_API

%include "gnuradio.i"
%include "stdint.i"

//load generated python docstrings
%include "analog_swig_doc.i"

%{
#include <gnuradio/blocks/control_loop.h>
%}
%include <gnuradio/blocks/control_loop.h>

%{
#include "gnuradio/analog/cpm.h"
#include "gnuradio/analog/noise_type.h"
#include "gnuradio/analog/agc_cc.h"
#include "gnuradio/analog/agc_ff.h"
#include "gnuradio/analog/agc2_cc.h"
#include "gnuradio/analog/agc2_ff.h"
#include "gnuradio/analog/agc3_cc.h"
#include "gnuradio/analog/cpfsk_bc.h"
#include "gnuradio/analog/ctcss_squelch_ff.h"
#include "gnuradio/analog/dpll_bb.h"
#include "gnuradio/analog/feedforward_agc_cc.h"
#include "gnuradio/analog/fmdet_cf.h"
#include "gnuradio/analog/frequency_modulator_fc.h"
#include "gnuradio/analog/fastnoise_source_s.h"
#include "gnuradio/analog/fastnoise_source_i.h"
#include "gnuradio/analog/fastnoise_source_f.h"
#include "gnuradio/analog/fastnoise_source_c.h"
#include "gnuradio/analog/noise_source_s.h"
#include "gnuradio/analog/noise_source_i.h"
#include "gnuradio/analog/noise_source_f.h"
#include "gnuradio/analog/noise_source_c.h"
#include "gnuradio/analog/phase_modulator_fc.h"
#include "gnuradio/analog/pll_carriertracking_cc.h"
#include "gnuradio/analog/pll_freqdet_cf.h"
#include "gnuradio/analog/pll_refout_cc.h"
#include "gnuradio/analog/probe_avg_mag_sqrd_c.h"
#include "gnuradio/analog/probe_avg_mag_sqrd_cf.h"
#include "gnuradio/analog/probe_avg_mag_sqrd_f.h"
#include "gnuradio/analog/pwr_squelch_cc.h"
#include "gnuradio/analog/pwr_squelch_ff.h"
#include "gnuradio/analog/quadrature_demod_cf.h"
#include "gnuradio/analog/rail_ff.h"
#include "gnuradio/analog/sig_source_s.h"
#include "gnuradio/analog/sig_source_i.h"
#include "gnuradio/analog/sig_source_f.h"
#include "gnuradio/analog/sig_source_c.h"
#include "gnuradio/analog/sig_source_waveform.h"
#include "gnuradio/analog/simple_squelch_cc.h"
#include "gnuradio/analog/squelch_base_cc.h"
#include "gnuradio/analog/squelch_base_ff.h"
#include "gnuradio/analog/random_uniform_source_b.h"
#include "gnuradio/analog/random_uniform_source_s.h"
#include "gnuradio/analog/random_uniform_source_i.h"
%}

%include "gnuradio/analog/cpm.h"
%include "gnuradio/analog/noise_type.h"
%include "gnuradio/analog/agc_cc.h"
%include "gnuradio/analog/agc_ff.h"
%include "gnuradio/analog/agc2_cc.h"
%include "gnuradio/analog/agc2_ff.h"
%include "gnuradio/analog/agc3_cc.h"
%include "gnuradio/analog/cpfsk_bc.h"
%include "gnuradio/analog/ctcss_squelch_ff.h"
%include "gnuradio/analog/dpll_bb.h"
%include "gnuradio/analog/feedforward_agc_cc.h"
%include "gnuradio/analog/fmdet_cf.h"
%include "gnuradio/analog/frequency_modulator_fc.h"
%include "gnuradio/analog/fastnoise_source_s.h"
%include "gnuradio/analog/fastnoise_source_i.h"
%include "gnuradio/analog/fastnoise_source_f.h"
%include "gnuradio/analog/fastnoise_source_c.h"
%include "gnuradio/analog/noise_source_s.h"
%include "gnuradio/analog/noise_source_i.h"
%include "gnuradio/analog/noise_source_f.h"
%include "gnuradio/analog/noise_source_c.h"
%include "gnuradio/analog/phase_modulator_fc.h"
%include "gnuradio/analog/pll_carriertracking_cc.h"
%include "gnuradio/analog/pll_freqdet_cf.h"
%include "gnuradio/analog/pll_refout_cc.h"
%include "gnuradio/analog/probe_avg_mag_sqrd_c.h"
%include "gnuradio/analog/probe_avg_mag_sqrd_cf.h"
%include "gnuradio/analog/probe_avg_mag_sqrd_f.h"
%include "gnuradio/analog/pwr_squelch_cc.h"
%include "gnuradio/analog/pwr_squelch_ff.h"
%include "gnuradio/analog/quadrature_demod_cf.h"
%include "gnuradio/analog/rail_ff.h"
%include "gnuradio/analog/sig_source_s.h"
%include "gnuradio/analog/sig_source_i.h"
%include "gnuradio/analog/sig_source_f.h"
%include "gnuradio/analog/sig_source_c.h"
%include "gnuradio/analog/sig_source_waveform.h"
%include "gnuradio/analog/simple_squelch_cc.h"
%include "gnuradio/analog/squelch_base_cc.h"
%include "gnuradio/analog/squelch_base_ff.h"
%include "gnuradio/analog/random_uniform_source_b.h"
%include "gnuradio/analog/random_uniform_source_s.h"
%include "gnuradio/analog/random_uniform_source_i.h"

GR_SWIG_BLOCK_MAGIC2(analog, agc_cc);
GR_SWIG_BLOCK_MAGIC2(analog, agc_ff);
GR_SWIG_BLOCK_MAGIC2(analog, agc2_cc);
GR_SWIG_BLOCK_MAGIC2(analog, agc2_ff);
GR_SWIG_BLOCK_MAGIC2(analog, agc3_cc);
GR_SWIG_BLOCK_MAGIC2(analog, cpfsk_bc);
GR_SWIG_BLOCK_MAGIC2(analog, ctcss_squelch_ff);
GR_SWIG_BLOCK_MAGIC2(analog, dpll_bb);
GR_SWIG_BLOCK_MAGIC2(analog, feedforward_agc_cc);
GR_SWIG_BLOCK_MAGIC2(analog, fmdet_cf);
GR_SWIG_BLOCK_MAGIC2(analog, frequency_modulator_fc);
GR_SWIG_BLOCK_MAGIC2(analog, fastnoise_source_s);
GR_SWIG_BLOCK_MAGIC2(analog, fastnoise_source_i);
GR_SWIG_BLOCK_MAGIC2(analog, fastnoise_source_f);
GR_SWIG_BLOCK_MAGIC2(analog, fastnoise_source_c);
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
GR_SWIG_BLOCK_MAGIC2(analog, quadrature_demod_cf);
GR_SWIG_BLOCK_MAGIC2(analog, rail_ff);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_s);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_i);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_f);
GR_SWIG_BLOCK_MAGIC2(analog, sig_source_c);
GR_SWIG_BLOCK_MAGIC2(analog, simple_squelch_cc);
GR_SWIG_BLOCK_MAGIC2(analog, random_uniform_source_b);
GR_SWIG_BLOCK_MAGIC2(analog, random_uniform_source_s);
GR_SWIG_BLOCK_MAGIC2(analog, random_uniform_source_i);
