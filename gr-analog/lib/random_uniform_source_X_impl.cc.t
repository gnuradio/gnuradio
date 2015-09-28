/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "@IMPL_NAME@.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace analog {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(int minimum, int maximum, int seed)
    {
      return gnuradio::get_initial_sptr(new @IMPL_NAME@(minimum, maximum, seed));
    }

    @IMPL_NAME@::@IMPL_NAME@(int minimum, int maximum, int seed)
    : sync_block("@BASE_NAME@",
            io_signature::make(0, 0, 0),
            io_signature::make(1, 1, sizeof(@TYPE@)))
    {
      d_rng = new gr::random(seed, minimum, maximum);
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
      delete d_rng;
    }

    int
    @IMPL_NAME@::random_value()
    {
      return d_rng->ran_int();
    }

    int
    @IMPL_NAME@::work(int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items)
    {
      @TYPE@ *out = (@TYPE@*)output_items[0];

      for(int i = 0; i < noutput_items; i++){
        *out++ = (@TYPE@) random_value();
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }








  } /* namespace analog */
} /* namespace gr */
