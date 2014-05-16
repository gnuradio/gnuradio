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

#include "async_encoder_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <stdio.h>

namespace gr {
  namespace fec {

    async_encoder::sptr
    async_encoder::make(generic_encoder::sptr my_encoder)
    {
      return gnuradio::get_initial_sptr
        (new async_encoder_impl(my_encoder));
    }

    async_encoder_impl::async_encoder_impl(generic_encoder::sptr my_encoder)
      : block("async_encoder",
              io_signature::make(0,0,0),
              io_signature::make(0,0,0)),
        d_input_item_size(sizeof(char)), d_output_item_size(sizeof(char))
    {
      d_in_port = pmt::mp("in");
      d_out_port = pmt::mp("out");

      d_encoder = my_encoder;
      message_port_register_in(d_in_port);
      message_port_register_out(d_out_port);
      set_msg_handler(d_in_port, boost::bind(&async_encoder_impl::encode, this ,_1) );
    }

    async_encoder_impl::~async_encoder_impl()
    {
    }

    void
    async_encoder_impl::encode(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bytes(pmt::cdr(msg));

      int nbytes = pmt::length(bytes);
      int nbits = 8*nbytes;
      size_t o0(0);
      const uint8_t* bytes_in = pmt::u8vector_elements(bytes, o0);
      uint8_t* bits_in = (uint8_t*)volk_malloc(nbits*sizeof(uint8_t),
                                               volk_get_alignment());

      // Stolen from unpack_k_bits
      int n = 0;
      for(int i = 0; i < nbytes; i++) {
        uint8_t t = bytes_in[i];
        for(int j = 7; j >= 0; j--)
          bits_in[n++] = (t >> j) & 0x01;
      }

      d_encoder->set_frame_size(nbits);

      int nbits_out = d_encoder->get_output_size();
      int nbytes_out = nbits_out/8;

      // get pmt buffer pointers
      uint8_t* bits_out = (uint8_t*)volk_malloc(nbits_out*sizeof(uint8_t),
                                                volk_get_alignment());
      uint8_t* bytes_out = (uint8_t*)volk_malloc(nbytes_out*sizeof(uint8_t),
                                                 volk_get_alignment());

      // ENCODE!
      d_encoder->generic_work((void*)bits_in, (void*)bits_out);

      // Stolen from pack_k_bits
      for(int i = 0; i < nbytes_out; i++) {
        bytes_out[i] = 0x00;
        for(int j = 0; j < 8; j++) {
          bytes_out[i] |= (0x01 & bits_out[i*8+j])<<(8-j-1);
        }
      }

      //pmt::pmt_t outvec = pmt::init_u8vector(nouts, u8out);
      pmt::pmt_t outvec = pmt::init_u8vector(nbytes_out, bytes_out);
      pmt::pmt_t msg_pair = pmt::cons(meta, outvec);
      message_port_pub(d_out_port, msg_pair);

      volk_free(bits_in);
      volk_free(bits_out);
      volk_free(bytes_out);
    }

    int
    async_encoder_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

  } /* namespace fec */
} /* namespace gr */
