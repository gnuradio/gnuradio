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

// WARNING: this file is machine generated.  Edits will be over written

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_noise_source_c.h>
#include <gr_io_signature.h>
#include <stdexcept>


gr_noise_source_c_sptr 
gr_make_noise_source_c (gr_noise_type_t type, float ampl, long seed)
{
  return gnuradio::get_initial_sptr(new gr_noise_source_c (type, ampl, seed));
}


gr_noise_source_c::gr_noise_source_c (gr_noise_type_t type, float ampl, long seed)
  : gr_sync_block ("noise_source_c",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (1, 1, sizeof (gr_complex))),
    d_type (type),
    d_ampl (ampl),
    d_rng (seed)
{
}

int
gr_noise_source_c::work (int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items)
{
  gr_complex *out = (gr_complex *) output_items[0];
  
  switch (d_type){
#if 1	// complex?

  case GR_UNIFORM:
    for (int i = 0; i < noutput_items; i++)
      out[i] = gr_complex (d_ampl * ((d_rng.ran1 () * 2.0) - 1.0),
			   d_ampl * ((d_rng.ran1 () * 2.0) - 1.0));
    break;

  case GR_GAUSSIAN:
    for (int i = 0; i < noutput_items; i++)
      out[i] = d_ampl * d_rng.rayleigh_complex ();
    break;

#else			// nope...

  case GR_UNIFORM:
    for (int i = 0; i < noutput_items; i++)
      out[i] = (gr_complex)(d_ampl * ((d_rng.ran1 () * 2.0) - 1.0));
    break;

  case GR_GAUSSIAN:
    for (int i = 0; i < noutput_items; i++)
      out[i] = (gr_complex)(d_ampl * d_rng.gasdev ());
    break;
    
  case GR_LAPLACIAN:
    for (int i = 0; i < noutput_items; i++)
      out[i] = (gr_complex)(d_ampl * d_rng.laplacian ());
    break;
    
  case GR_IMPULSE:	// FIXME changeable impulse settings
    for (int i = 0; i < noutput_items; i++)
      out[i] = (gr_complex)(d_ampl * d_rng.impulse (9));
    break;
#endif
    
  default:
    throw std::runtime_error ("invalid type");
  }

  return noutput_items;
}
