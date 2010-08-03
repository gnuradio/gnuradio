/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_vco_f.h>
#include <gr_io_signature.h>
#include <math.h>

gr_vco_f_sptr
gr_make_vco_f(double sampling_rate, double sensitivity, double amplitude)
{
  return gnuradio::get_initial_sptr(new gr_vco_f(sampling_rate, sensitivity, amplitude));
}


gr_vco_f::gr_vco_f(double sampling_rate, double sensitivity, double amplitude)
  : gr_sync_block("vco_f",
		  gr_make_io_signature(1, 1, sizeof(float)),
		  gr_make_io_signature(1, 1, sizeof(float))),
    d_sampling_rate(sampling_rate), d_sensitivity(sensitivity), d_amplitude(amplitude),
    d_k(d_sensitivity/d_sampling_rate)
{
}

int
gr_vco_f::work (int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
  const float *input = (const float *)input_items[0];
  float *output = (float *)output_items[0];

  d_vco.cos(output, input, noutput_items, d_k, d_amplitude);
  
  return noutput_items;
}
