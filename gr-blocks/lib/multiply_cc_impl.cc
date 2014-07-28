/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <multiply_cc_impl.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    multiply_cc::sptr multiply_cc::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr(new multiply_cc_impl(vlen));
    }

    multiply_cc_impl::multiply_cc_impl(size_t vlen)
      : sync_block("multiply_cc",
		      io_signature::make (1, -1, sizeof(gr_complex)*vlen),
		      io_signature::make (1,  1, sizeof(gr_complex)*vlen)),
	d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
    }

    int
    multiply_cc_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];
      int noi = d_vlen*noutput_items;

      memcpy(out, input_items[0], noi*sizeof(gr_complex));
      for(size_t i = 1; i < input_items.size(); i++)
        volk_32fc_x2_multiply_32fc(out, out, (gr_complex*)input_items[i], noi);

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
