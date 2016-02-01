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

#include "tagged_decoder_impl.h"
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    tagged_decoder::sptr
    tagged_decoder::make(generic_decoder::sptr my_decoder,
                         size_t input_item_size,
                         size_t output_item_size,
                         const std::string &lengthtagname,
                         int mtu)
    {
      return gnuradio::get_initial_sptr
        ( new tagged_decoder_impl(my_decoder, input_item_size,
                                  output_item_size, lengthtagname,
                                  mtu));
    }

    tagged_decoder_impl::tagged_decoder_impl(generic_decoder::sptr my_decoder,
                                             size_t input_item_size,
                                             size_t output_item_size,
                                             const std::string &lengthtagname,
                                             int mtu)
      : tagged_stream_block("fec_tagged_decoder",
                            io_signature::make(1, 1, input_item_size),
                            io_signature::make(1, 1, output_item_size),
                            lengthtagname),
      d_input_item_size(input_item_size), d_output_item_size(output_item_size),
      d_mtu(mtu)
    {
      d_decoder = my_decoder;
      d_decoder->set_frame_size(d_mtu*8);

      set_relative_rate(d_decoder->rate());
    }

    int
    tagged_decoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      if((ninput_items[0]*d_decoder->rate()) > (d_mtu*8)) {
        throw std::runtime_error("tagged_encoder: received frame is larger than MTU.");
      }
      d_decoder->set_frame_size(round(ninput_items[0]*d_decoder->rate()));
      return d_decoder->get_output_size();
    }

    tagged_decoder_impl::~tagged_decoder_impl()
    {
    }

    int
    tagged_decoder_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      const unsigned char *in = (unsigned char*)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      GR_LOG_DEBUG(d_debug_logger, boost::format("%1%, %2%, %3%")      \
                   % noutput_items % ninput_items[0] % d_decoder->get_output_size());

      d_decoder->generic_work((void*)in, (void*)out);

      return d_decoder->get_output_size();
    }

  } /* namespace fec */
} /* namespace gr */
