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

#ifndef INCLUDED_FEC_ENCODER_IMPL_H
#define INCLUDED_FEC_ENCODER_IMPL_H

#include <gnuradio/fec/encoder.h>

namespace gr {
  namespace fec {

    class FEC_API encoder_impl : public encoder
    {
    private:
      generic_encoder::sptr d_encoder;
      size_t d_input_item_size;
      size_t d_output_item_size;
      size_t d_input_size;
      size_t d_output_size;

    public:
      encoder_impl(generic_encoder::sptr my_encoder,
                   size_t input_item_size,
                   size_t output_item_size);
      ~encoder_impl();

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
      int fixed_rate_ninput_to_noutput(int ninput);
      int fixed_rate_noutput_to_ninput(int noutput);
      void forecast(int noutput_items,
                    gr_vector_int& ninput_items_required);
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ENCODER_IMPL_H */
