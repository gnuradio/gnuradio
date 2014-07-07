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

#include "add_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    add_ff::sptr add_ff::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr(new add_ff_impl(vlen));
    }

    add_ff_impl::add_ff_impl(size_t vlen)
      : sync_block("add_ff",
		      io_signature::make (1, -1, sizeof(float)*vlen),
		      io_signature::make (1,  1, sizeof(float)*vlen)),
	d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    int
    add_ff_impl::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      float *out = (float *) output_items[0];
      int noi = d_vlen*noutput_items;

      memcpy(out, input_items[0], noi*sizeof(float));
      for(size_t i = 1; i < input_items.size(); i++)
        volk_32f_x2_add_32f(out, out, (const float*)input_items[i], noi);
      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
