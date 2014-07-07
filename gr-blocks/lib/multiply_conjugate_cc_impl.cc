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

#include <multiply_conjugate_cc_impl.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    multiply_conjugate_cc::sptr multiply_conjugate_cc::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr(new multiply_conjugate_cc_impl(vlen));
    }

    multiply_conjugate_cc_impl::multiply_conjugate_cc_impl(size_t vlen)
      : sync_block("multiply_conjugate_cc",
		      io_signature::make (2, 2, sizeof(gr_complex)*vlen),
		      io_signature::make (1, 1, sizeof(gr_complex)*vlen)),
	d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
    }

    int
    multiply_conjugate_cc_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      gr_complex *in0 = (gr_complex *) input_items[0];
      gr_complex *in1 = (gr_complex *) input_items[1];
      gr_complex *out = (gr_complex *) output_items[0];
      int noi = d_vlen*noutput_items;

      volk_32fc_x2_multiply_conjugate_32fc(out, in0, in1, noi);

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
