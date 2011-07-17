/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <digital_cma_equalizer_cc.h>
#include <cstdio>

digital_cma_equalizer_cc_sptr
digital_make_cma_equalizer_cc(int num_taps, float modulus, float mu, int sps)
{
  return gnuradio::get_initial_sptr(new digital_cma_equalizer_cc(num_taps, modulus,
								 mu, sps));
}

digital_cma_equalizer_cc::digital_cma_equalizer_cc(int num_taps, float modulus,
						   float mu, int sps)
  : gr_adaptive_fir_ccc("cma_equalizer_cc", sps,
			std::vector<gr_complex>(num_taps, gr_complex(0,0)))
{
  set_modulus(modulus);
  set_gain(mu);
  if (num_taps > 0)
    d_taps[0] = 1.0;
}
