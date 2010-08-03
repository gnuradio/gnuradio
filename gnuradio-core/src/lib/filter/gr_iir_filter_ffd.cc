/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

#include <gr_iir_filter_ffd.h>
#include <gr_io_signature.h>
#include <stdio.h>


gr_iir_filter_ffd_sptr 
gr_make_iir_filter_ffd (const std::vector<double> &fftaps,
			const std::vector<double> &fbtaps) throw (std::invalid_argument)
{
  return gnuradio::get_initial_sptr(new gr_iir_filter_ffd (fftaps, fbtaps));
}

gr_iir_filter_ffd::gr_iir_filter_ffd (const std::vector<double> &fftaps,
				      const std::vector<double> &fbtaps) throw (std::invalid_argument)

  : gr_sync_block ("iir_filter_ffd",
		   gr_make_io_signature (1, 1, sizeof (float)),
		   gr_make_io_signature (1, 1, sizeof (float))),
    d_iir (fftaps, fbtaps),
    d_updated (false)
{
  // fprintf (stderr, "gr_iir_filter_ffd::ctor\n");
}

gr_iir_filter_ffd::~gr_iir_filter_ffd ()
{
  // nop
}

void
gr_iir_filter_ffd::set_taps (const std::vector<double> &fftaps,
			     const std::vector<double> &fbtaps) throw (std::invalid_argument)
{
  
  d_new_fftaps = fftaps;
  d_new_fbtaps = fbtaps;
  d_updated = true;
}

int
gr_iir_filter_ffd::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  float *out = (float *) output_items[0];


  if (d_updated){
    d_iir.set_taps (d_new_fftaps, d_new_fbtaps);
    d_updated = false;
  }

  d_iir.filter_n (out, in, noutput_items);
  return noutput_items;
};
