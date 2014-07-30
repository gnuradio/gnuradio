/* -*- c++ -*- */
/*
 * Copyright 2006,2010-2012,2014 Free Software Foundation, Inc.
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

#include "binary_slicer_fb_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
  namespace digital {

    binary_slicer_fb::sptr binary_slicer_fb::make()
    {
      return gnuradio::get_initial_sptr(new binary_slicer_fb_impl());
    }

    binary_slicer_fb_impl::binary_slicer_fb_impl()
      : sync_block("binary_slicer_fb",
                   io_signature::make(1, 1, sizeof(float)),
                   io_signature::make(1, 1, sizeof(int8_t)))
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(int8_t);
      set_alignment(std::max(1,alignment_multiple));
    }

    binary_slicer_fb_impl::~binary_slicer_fb_impl()
    {
    }

    int
    binary_slicer_fb_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      int8_t *out = (int8_t *)output_items[0];

      volk_32f_binary_slicer_8i(out, in, noutput_items);

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
