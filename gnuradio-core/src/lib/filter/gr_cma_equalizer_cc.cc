/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <gr_cma_equalizer_cc.h>

gr_cma_equalizer_cc_sptr
gr_make_cma_equalizer_cc(int num_taps, float modulus, float mu)
{
  return gr_cma_equalizer_cc_sptr(new gr_cma_equalizer_cc(num_taps, modulus, mu));
}

gr_cma_equalizer_cc::gr_cma_equalizer_cc(int num_taps, float modulus, float mu)
  : gr_adaptive_fir_ccf("cma_equalizer_cc", 1, std::vector<float>(num_taps)),
    d_modulus(modulus), d_mu(mu)
{
  d_taps[0] = 1.0;
}

