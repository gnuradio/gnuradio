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
#include <config.h>
#endif

#include <gr_probe_density_b.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>

gr_probe_density_b_sptr 
gr_make_probe_density_b(double alpha)
{
  return gnuradio::get_initial_sptr(new gr_probe_density_b(alpha));
}

gr_probe_density_b::gr_probe_density_b(double alpha)
  : gr_sync_block("density_b",
		  gr_make_io_signature(1, 1, sizeof(char)),
		  gr_make_io_signature(0, 0, 0))
{
  set_alpha(alpha);
  d_density = 1.0;
}

gr_probe_density_b::~gr_probe_density_b()
{
}

int 
gr_probe_density_b::work(int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  const char *in = (const char *)input_items[0];

  for (int i = 0; i < noutput_items; i++)
    d_density = d_alpha*(double)in[i] + d_beta*d_density;

  return noutput_items;
}

void
gr_probe_density_b::set_alpha(double alpha)
{
  d_alpha = alpha;
  d_beta = 1.0-d_alpha;
}

