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

#ifndef INCLUDED_FEC_ASYNC_ENCODER_IMPL_H
#define INCLUDED_FEC_ASYNC_ENCODER_IMPL_H

#include <gnuradio/fec/async_encoder.h>
#include <gnuradio/blocks/unpack_k_bits.h>
#include <gnuradio/blocks/pack_k_bits.h>

namespace gr {
  namespace fec {

    class FEC_API async_encoder_impl : public async_encoder
    {
    private:
      generic_encoder::sptr d_encoder;
      size_t d_input_item_size;
      size_t d_output_item_size;

      pmt::pmt_t d_in_port;
      pmt::pmt_t d_out_port;

      blocks::kernel::unpack_k_bits *d_unpack;
      blocks::kernel::pack_k_bits   *d_pack;

      bool d_packed;
      bool d_rev_unpack;
      bool d_rev_pack;
      int d_mtu;

      uint8_t* d_bits_in;
      uint8_t* d_bits_out;

      void encode_packed(pmt::pmt_t msg);
      void encode_unpacked(pmt::pmt_t msg);

    public:
      async_encoder_impl(generic_encoder::sptr my_encoder,
                         bool packed=false,
                         bool rev_unpack=true, bool rev_pack=true,
                         int mtu=1500);
      ~async_encoder_impl();

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_ASYNC_ENCODER_IMPL_H */
