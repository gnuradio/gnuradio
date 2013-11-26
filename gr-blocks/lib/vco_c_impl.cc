/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include "vco_c_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>

namespace gr {
  namespace blocks {

    vco_c::sptr
    vco_c::make(double sampling_rate, double sensitivity, double amplitude)
    {
      return gnuradio::get_initial_sptr
        (new vco_c_impl(sampling_rate, sensitivity, amplitude));
    }

    vco_c_impl::vco_c_impl(double sampling_rate, double sensitivity, double amplitude)
      : sync_block("vco_c",
                   io_signature::make(1, 1, sizeof(float)),
                   io_signature::make(1, 1, sizeof(gr_complex))),
        d_sampling_rate(sampling_rate), d_sensitivity(sensitivity),
        d_amplitude(amplitude), d_k(d_sensitivity/d_sampling_rate)
    {
    }

    vco_c_impl::~vco_c_impl()
    {
    }

    int
    vco_c_impl::work(int noutput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items)
    {
      const float *input = (const float*)input_items[0];
      gr_complex *output = (gr_complex*)output_items[0];

      d_vco.sincos(output, input, noutput_items, d_k, d_amplitude);

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
