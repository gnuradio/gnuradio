/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include "glfsr_source_b_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
  namespace digital {

    glfsr_source_b::sptr
    glfsr_source_b::make(int degree, bool repeat, int mask, int seed)
    {
      return gnuradio::get_initial_sptr
	(new glfsr_source_b_impl(degree, repeat, mask, seed));
    }

    glfsr_source_b_impl::glfsr_source_b_impl(int degree, bool repeat,
					     int mask, int seed)
      : sync_block("glfsr_source_b",
		      io_signature::make(0, 0, 0),
		      io_signature::make(1, 1, sizeof(unsigned char))),
	d_repeat(repeat), d_index(0)
    {
      if(degree < 1 || degree > 32)
	throw std::runtime_error("glfsr_source_b_impl: degree must be between 1 and 32 inclusive");
      d_length = (unsigned int)((1ULL << degree)-1);

      if(mask == 0)
	mask = glfsr::glfsr_mask(degree);
      d_glfsr = new glfsr(mask, seed);
    }

    glfsr_source_b_impl::~glfsr_source_b_impl()
    {
      delete d_glfsr;
    }

    int
    glfsr_source_b_impl::mask() const
    {
      return d_glfsr->mask();
    }

    int
    glfsr_source_b_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      char *out = (char*)output_items[0];
      if((d_index > d_length) && d_repeat == false)
	return -1; /* once through the sequence */

      int i;
      for(i = 0; i < noutput_items; i++) {
	out[i] = d_glfsr->next_bit();
	d_index++;
	if(d_index > d_length && d_repeat == false)
	  break;
      }

      return i;
    }

  } /* namespace digital */
} /* namespace gr */
