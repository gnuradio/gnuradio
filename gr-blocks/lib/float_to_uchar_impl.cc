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

#include "float_to_uchar_impl.h"
#include "float_array_to_uchar.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    float_to_uchar::sptr float_to_uchar::make()
    {
      return gnuradio::get_initial_sptr(new float_to_uchar_impl());
    }

    float_to_uchar_impl::float_to_uchar_impl()
      : sync_block("float_to_uchar",
		      io_signature::make (1, 1, sizeof(float)),
		      io_signature::make (1, 1, sizeof(unsigned char)))
    {
    }

    int
    float_to_uchar_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const float *in = (const float *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      float_array_to_uchar(in, out, noutput_items);

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
