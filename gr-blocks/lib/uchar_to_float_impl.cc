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

#include "uchar_to_float_impl.h"
#include "uchar_array_to_float.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    uchar_to_float::sptr uchar_to_float::make()
    {
      return gnuradio::get_initial_sptr(new uchar_to_float_impl());
    }

    uchar_to_float_impl::uchar_to_float_impl()
      : sync_block("uchar_to_float",
		      io_signature::make (1, 1, sizeof(unsigned char)),
		      io_signature::make (1, 1, sizeof(float)))
    {
    }

    int
    uchar_to_float_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      float *out = (float *) output_items[0];

      uchar_array_to_float (in, out, noutput_items);

      return noutput_items;
    }

  } /* namespace blocks */
}/* namespace gr */
