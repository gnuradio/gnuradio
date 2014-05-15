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

#include "async_decoder_impl.h"
#include <gnuradio/io_signature.h>
#include <stdio.h>


namespace gr {
  namespace fec {

    async_decoder::sptr
    async_decoder::make(generic_decoder::sptr my_decoder,
                        size_t input_item_size, size_t output_item_size)
    {
      return gnuradio::get_initial_sptr
        (new async_decoder_impl(my_decoder, input_item_size, output_item_size));
    }

    async_decoder_impl::async_decoder_impl(generic_decoder::sptr my_decoder,
                                           size_t input_item_size,
                                           size_t output_item_size)
      : block("async_decoder",
              io_signature::make(0,0,0),
              io_signature::make(0,0,0)),
        d_input_item_size(input_item_size), d_output_item_size(output_item_size)
    {
      d_decoder = my_decoder;
      message_port_register_in(pmt::mp("pdus"));
      message_port_register_out(pmt::mp("pdus"));
      set_msg_handler(pmt::mp("pdus"), boost::bind(&async_decoder_impl::decode, this ,_1));
    }

    async_decoder_impl::~async_decoder_impl()
    {
    }

    void
    async_decoder_impl::decode(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bits(pmt::cdr(msg));

      int nbits = pmt::length(bits);
      int blksize_in = d_decoder->get_input_size();
      int blksize_out = d_decoder->get_output_size();
      int nblocks = nbits/blksize_in;

      // nbits sanity check
      if(nbits % blksize_in != 0){
        throw std::runtime_error((boost::format("nbits_in %% blksize_in != 0 in fec_decoder_asyn! %d bits in, %d blksize") \
                                  % nbits % blksize_in).str());
      }

      printf("decoder :: blksize_in = %d, blksize_out = %d\n", blksize_in, blksize_out);

      pmt::pmt_t outvec(pmt::make_u8vector(blksize_out * nblocks, 0xFF));
      printf("decoder :: outvec len = %d (%d blks)\n", blksize_out * nblocks, nblocks);

      // get pmt buffer pointers
      size_t o0(0);
      const float* f32in = pmt::f32vector_elements(bits, o0);
      uint8_t* u8out = pmt::u8vector_writable_elements(outvec, o0);

      // loop over n fec blocks
      for(int i = 0; i < nblocks; i++) {
        size_t offset_in = i * blksize_in;
        size_t offset_out = i * blksize_out;

        // DECODE!
        d_decoder->generic_work((void*)&f32in[offset_in], (void*)&u8out[offset_out]);
      }

      message_port_pub(pmt::mp("pdus"),  pmt::cons(meta, outvec));
    }

    int
    async_decoder_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

  } /* namespace fec */
} /* namespace gr */
