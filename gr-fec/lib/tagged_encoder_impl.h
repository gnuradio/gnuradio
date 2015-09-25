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

#ifndef INCLUDED_FEC_TAGGED_ENCODER_IMPL_H
#define INCLUDED_FEC_TAGGED_ENCODER_IMPL_H

#include <gnuradio/fec/tagged_encoder.h>

namespace gr {
  namespace fec {

    class FEC_API tagged_encoder_impl : public tagged_encoder
    {
    private:
      generic_encoder::sptr d_encoder;
      size_t d_input_item_size;
      size_t d_output_item_size;
      int d_mtu;

    public:
      tagged_encoder_impl(generic_encoder::sptr my_encoder,
                          size_t input_item_size,
                          size_t output_item_size,
                          const std::string &lengthtagname="packet_len",
                          int mtu=1500);
      ~tagged_encoder_impl();

      int work(int noutput_items,
               gr_vector_int& ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
      int calculate_output_stream_length(const gr_vector_int &ninput_items);
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_TAGGED_ENCODER_IMPL_H */
