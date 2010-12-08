/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gri_cpm.h>
#include <gr_cpmmod_bc.h>
#include <gr_io_signature.h>


// Shared pointer constructor
gr_cpmmod_bc_sptr
gr_make_cpmmod_bc(int type, float h, unsigned samples_per_sym, unsigned L, double beta)
{
  return gnuradio::get_initial_sptr(new gr_cpmmod_bc((cpm_type)type, h, samples_per_sym, L, beta));
}


// Parameters:
// - type (raised cosine, spectral raised cosine, rectangular, tamed fm, gmsk.
// - h (modulation index)
// - L (length of filter in symbols)
// - samples per symbol
// - beta (for gmsk: BT product, for the RC's: rolloff)
gr_cpmmod_bc::gr_cpmmod_bc(cpm_type type, double h, unsigned samples_per_sym,
							unsigned L, double beta)
  : gr_hier_block2("gr_cpmmod_bc",
		   gr_make_io_signature(1, 1, sizeof(char)),
		   gr_make_io_signature2(1, 2, sizeof(gr_complex), sizeof(float))),
	d_char_to_float(gr_make_char_to_float()),
	d_fm(gr_make_frequency_modulator_fc(M_TWOPI * h / samples_per_sym)),
	d_taps(generate_cpm_taps(type, samples_per_sym, L, beta)),
	d_pulse_shaper(gr_make_interp_fir_filter_fff(samples_per_sym, d_taps))
{
  switch (type) {
	  case CPM_LRC:
	  case CPM_LSRC:
	  case CPM_LREC:
	  case CPM_TFM:
	  case CPM_GMSK:
		  break;

	  default:
		  throw std::invalid_argument("invalid CPM type");
  }

  connect(self(), 0, d_char_to_float, 0);
  connect(d_char_to_float, 0, d_pulse_shaper, 0);
  connect(d_pulse_shaper, 0, d_fm, 0);
  connect(d_fm, 0, self(), 0);

  // FIXME is this valid? multiple outputs?
  connect(d_pulse_shaper, 0, self(), 1);
}

