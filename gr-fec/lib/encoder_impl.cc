/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#include "encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    encoder::sptr
    encoder::make(generic_encoder::sptr my_encoder,
                  size_t input_item_size,
                  size_t output_item_size)
    {
      return gnuradio::get_initial_sptr
        (new encoder_impl(my_encoder, input_item_size,
                          output_item_size));
    }

    encoder_impl::encoder_impl(generic_encoder::sptr my_encoder,
                               size_t input_item_size,
                               size_t output_item_size)
      : block("fec_encoder",
              io_signature::make(1, 1, input_item_size),
              io_signature::make(1, 1, output_item_size)),
        d_input_item_size(input_item_size),
        d_output_item_size(output_item_size)
    {
      set_fixed_rate(true);
      set_relative_rate((double)my_encoder->get_output_size()/(double)my_encoder->get_input_size());
      set_output_multiple(my_encoder->get_output_size());
      d_encoder = my_encoder;

      d_input_size = d_encoder->get_input_size()*d_input_item_size;
      d_output_size = d_encoder->get_output_size()*d_output_item_size;
    }

    encoder_impl::~encoder_impl()
    {
    }

    int
    encoder_impl::fixed_rate_ninput_to_noutput(int ninput)
    {
      return (int)(0.5 + ninput*relative_rate());
    }

    int
    encoder_impl::fixed_rate_noutput_to_ninput(int noutput)
    {
      return (int)(0.5 + noutput/relative_rate());
    }

    void
    encoder_impl::forecast(int noutput_items,
                           gr_vector_int& ninput_items_required)
    {
      ninput_items_required[0] = fixed_rate_noutput_to_ninput(noutput_items);
    }

    int
    encoder_impl::general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
      char *inbuffer  = (char*)input_items[0];
      char *outbuffer = (char*)output_items[0];

      for(int i = 0; i < noutput_items/output_multiple(); i++) {
        d_encoder->generic_work((void*)(inbuffer+(i*d_input_size)),
                                (void*)(outbuffer+(i*d_output_size)));
      }

      consume_each(fixed_rate_noutput_to_ninput(noutput_items));
      return noutput_items;
    }

  } /* namespace fec */
} /* namespace gr */
