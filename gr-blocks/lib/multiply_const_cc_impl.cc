/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <multiply_const_cc_impl.h>
#include <gr_io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    multiply_const_cc::sptr multiply_const_cc::make(gr_complex k, size_t vlen)
    {
      return gnuradio::get_initial_sptr(new multiply_const_cc_impl(k, vlen));
    }

    multiply_const_cc_impl::multiply_const_cc_impl(gr_complex k, size_t vlen)
      : gr_sync_block ("multiply_const_cc",
		       gr_make_io_signature (1, 1, sizeof (gr_complex)*vlen),
		       gr_make_io_signature (1, 1, sizeof (gr_complex)*vlen)),
	d_k(k), d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));
    }

    int
    multiply_const_cc_impl::work(int noutput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int noi = d_vlen*noutput_items;

      if(is_unaligned()) {
	volk_32fc_s32fc_multiply_32fc_u(out, in, d_k, noi);
      }
      else {
	volk_32fc_s32fc_multiply_32fc_a(out, in, d_k, noi);
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
