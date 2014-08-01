/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include <sub_ff_impl.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    sub_ff::sptr
    sub_ff::make(size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new sub_ff_impl(vlen));
    }

    sub_ff_impl::sub_ff_impl(size_t vlen)
      : sync_block("@sub_ff",
                   io_signature::make(1, -1, sizeof(float)*vlen),
                   io_signature::make(1,  1, sizeof(float)*vlen)),
      d_vlen(vlen)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    int
    sub_ff_impl::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      float *out = (float*)output_items[0];

      int ninputs = input_items.size();
      int noi = noutput_items*d_vlen;

      //for(size_t i = 0; i < noutput_items*d_vlen; i++){
      //  @I_TYPE@ acc = ((@I_TYPE@ *) input_items[0])[i];
      //  for (int j = 1; j < ninputs; j++)
      //    acc -= ((@I_TYPE@ *) input_items[j])[i];
      //
      //  *optr++ = (@O_TYPE@) acc;
      //}

      const float *in = (const float*)input_items[0];
      memcpy(out, in, noi*sizeof(float));
      for(int i = 1; i < ninputs; i++) {
        in = (const float*)input_items[i];
        volk_32f_x2_subtract_32f(out, out, in, noi);
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
