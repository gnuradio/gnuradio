/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_top_block.h>
#include <gr_char_to_float.h>
#include <gr_interp_fir_filter_fff.h>
#include <gr_frequency_modulator_fc.h>


class gr_cpmmod_bc;
typedef boost::shared_ptr<gr_cpmmod_bc> gr_cpmmod_bc_sptr;

enum gr_cpmmod_bc::cpm_type;

gr_cpmmod_bc_sptr
gr_make_cpmmod_bc(int type, float h, unsigned samples_per_sym, unsigned L, double beta=0.3);

/*!
 * \brief Generic CPM modulator
 *
 * \ingroup modulation_blk
 *
 * The input of this block are symbols from an M-ary alphabet
 * \pm1, \pm3, ..., \pm(M-1). Usually, M = 2 and therefore, the
 * valid inputs are \pm1.
 * The output is the phase-modulated signal.
 */
class gr_cpmmod_bc : public gr_hier_block2
{
	friend gr_cpmmod_bc_sptr gr_make_cpmmod_bc(int type, float h, unsigned samples_per_sym, unsigned L, double beta);
	gr_cpmmod_bc(int type, float h, unsigned samples_per_sym, unsigned L, double beta);

	gr_char_to_float_sptr d_char_to_float;
	gr_interp_fir_filter_fff_sptr d_pulse_shaper;
	gr_frequency_modulator_fc_sptr d_fm;

	std::vector<float> d_taps;

 public:
	//! Return the phase response FIR taps
	 std::vector<float> get_taps() { return d_taps; };
};

