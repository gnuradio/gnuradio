/* -*- c++ -*- */
/*
 * Copyright 2008,2010 Free Software Foundation, Inc.
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

#include <gr_cpfsk_bc.h>
#include <gr_io_signature.h>
#include <gr_expj.h>

#define M_TWOPI (2*M_PI)

gr_cpfsk_bc_sptr 
gr_make_cpfsk_bc(float k, float ampl, int samples_per_sym)
{
  return gnuradio::get_initial_sptr(new gr_cpfsk_bc(k, ampl, samples_per_sym));
}

gr_cpfsk_bc::gr_cpfsk_bc(float k, float ampl, int samples_per_sym)
  : gr_sync_interpolator("cpfsk_bc",
		         gr_make_io_signature(1, 1, sizeof(char)),
		         gr_make_io_signature(1, 1, sizeof(gr_complex)),
			 samples_per_sym)
{
  d_samples_per_sym = samples_per_sym;
  d_freq = k*M_PI/samples_per_sym;
  d_ampl = ampl;
  d_phase = 0.0;
}

gr_cpfsk_bc::~gr_cpfsk_bc()
{
}

int 
gr_cpfsk_bc::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  const char *in = (const char *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  for (int i = 0; i < noutput_items/d_samples_per_sym; i++) {
    for (int j = 0; j < d_samples_per_sym; j++) {
      if (in[i] == 1)
	d_phase += d_freq;
      else
	d_phase -= d_freq;
      
      while (d_phase > M_TWOPI)
	d_phase -= M_TWOPI;
      while (d_phase < -M_TWOPI)
	d_phase += M_TWOPI;
      
      *out++ = gr_expj(d_phase)*d_ampl;
    }
  }

  return noutput_items;
}
