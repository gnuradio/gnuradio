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

#include "float_to_int_impl.h"
#include "float_array_to_int.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace blocks {

    float_to_int::sptr float_to_int::make(size_t vlen, float scale)
    {
      return gnuradio::get_initial_sptr(new float_to_int_impl(vlen, scale));
    }

    float_to_int_impl::float_to_int_impl(size_t vlen, float scale)
      : sync_block("float_to_int",
		      io_signature::make (1, 1, sizeof(float)*vlen),
		      io_signature::make (1, 1, sizeof(int)*vlen)),
	d_vlen(vlen), d_scale(scale)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(int);
      set_alignment(std::max(1, alignment_multiple));
    }

    int
    float_to_int_impl::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
    {
      // Disable the Volk for now. There is a problem for large 32-bit ints that
      // are not properly represented by the precisions of a single float, which
      // can cause wrapping from large, positive numbers to negative.
      // In gri_float_to_int, the value is first promoted to a 64-bit
      // value, clipped, then converted to a float.
#if 0
      const float *in = (const float *) input_items[0];
      int32_t *out = (int32_t *) output_items[0];

      volk_32f_s32f_convert_32i(out, in, d_scale, d_vlen*noutput_items);
#else
      const float *in = (const float *) input_items[0];
      int *out = (int *) output_items[0];

      float_array_to_int (in, out, d_scale, d_vlen*noutput_items);

#endif

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
