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
    async_encoder::make(generic_encoder::sptr my_encoder,
                        bool packed, bool rev_unpack, bool rev_pack,
                        int mtu)
    {
      return gnuradio::get_initial_sptr
        (new async_encoder_impl(my_encoder, packed, rev_unpack, rev_pack, mtu));
    }

    async_encoder_impl::async_encoder_impl(generic_encoder::sptr my_encoder,
                                           bool packed, bool rev_unpack, bool rev_pack,
                                           int mtu)
      : block("async_encoder",
              io_signature::make(0,0,0),
              io_signature::make(0,0,0)),
        d_input_item_size(sizeof(char)), d_output_item_size(sizeof(char))
    {
      d_in_port = pmt::mp("in");
      d_out_port = pmt::mp("out");

      d_encoder = my_encoder;

      d_packed = packed;
      d_rev_unpack = rev_unpack;
      d_rev_pack = rev_pack;
      d_mtu = mtu;

      message_port_register_in(d_in_port);
      message_port_register_out(d_out_port);

      if(d_packed) {
        set_msg_handler(d_in_port, boost::bind(&async_encoder_impl::encode_packed, this ,_1) );

        d_unpack = new blocks::kernel::unpack_k_bits(8);

        int max_bits_out = d_encoder->rate() * d_mtu * 8;
        d_bits_out = (uint8_t*)volk_malloc(max_bits_out*sizeof(uint8_t),
                                           volk_get_alignment());

      }
      else {
        set_msg_handler(d_in_port, boost::bind(&async_encoder_impl::encode_unpacked, this ,_1) );
      }

      if(d_packed || (strncmp(d_encoder->get_input_conversion(), "pack", 4) == 0)) {
        // encode_unpacked: if input conversion is 'pack', pack the input bits
        // encode_packed: used to repack the output
        d_pack = new blocks::kernel::pack_k_bits(8);

        // encode_unpacked: Holds packed bits in when input conversion is packed
        // encode_packed: holds the output bits of the encoder to be packed
        int max_bits_in = d_mtu*8;
        d_bits_in = (uint8_t*)volk_malloc(max_bits_in*sizeof(uint8_t),
                                          volk_get_alignment());
      }
    }

    async_encoder_impl::~async_encoder_impl()
    {
      if(d_packed) {
        delete d_unpack;
        volk_free(d_bits_out);
      }

      if(d_packed || (strncmp(d_encoder->get_input_conversion(), "pack", 4) == 0)) {
        delete d_pack;
        volk_free(d_bits_in);
      }
    }

    void
    async_encoder_impl::encode_unpacked(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bits(pmt::cdr(msg));

      size_t o0 = 0;
      int nbits_in = pmt::length(bits);
      if(nbits_in > (d_mtu*8)) {
        throw std::runtime_error("async_encoder: received message larger than the MTU.");
      }

      const uint8_t* bits_in = pmt::u8vector_elements(bits, o0);

      bool variable_framesize = d_encoder->set_frame_size(nbits_in);
      int nbits_out = 0;
      int nblocks = 1;
      if( variable_framesize ){
        nbits_out = d_encoder->get_output_size();
        } else {
        nblocks = nbits_in / d_encoder->get_input_size();
        if( nblocks * d_encoder->get_input_size() != nbits_in ){
            printf("nblocks: %u, in_block_size: %d, got_input_size: %d\n",
                nblocks, d_encoder->get_input_size(), nbits_in);
            throw std::runtime_error("input does not divide into code block size!");
            }
        nbits_out = nblocks * d_encoder->get_output_size();
        }


      // buffers for output bits to go to
      pmt::pmt_t outvec = pmt::make_u8vector(nbits_out, 0x00);
      uint8_t* bits_out = pmt::u8vector_writable_elements(outvec, o0);

      if(strncmp(d_encoder->get_input_conversion(), "pack", 4) == 0) {
        d_pack->pack(d_bits_in, bits_in, nbits_in/8);
        d_encoder->generic_work((void*)d_bits_in, (void*)bits_out);
      }
      else {
        for(int i=0; i<nblocks; i++){
            d_encoder->generic_work((void*) &bits_in[i*d_encoder->get_input_size()], (void*)&bits_out[i*d_encoder->get_output_size()]);
            }
      }

      pmt::pmt_t msg_pair = pmt::cons(meta, outvec);
      message_port_pub(d_out_port, msg_pair);
    }

    void
    async_encoder_impl::encode_packed(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bytes(pmt::cdr(msg));

      size_t o0 = 0;
      int nbytes_in = pmt::length(bytes);
      if(nbytes_in > d_mtu) {
        throw std::runtime_error("async_encoder: received message larger than the MTU.");
      }

      int nbits_in = 8*nbytes_in;
      const uint8_t* bytes_in = pmt::u8vector_elements(bytes, o0);

      d_encoder->set_frame_size(nbits_in);

      int nbits_out = d_encoder->get_output_size();
      int nbytes_out = nbits_out/8;

      if(strncmp(d_encoder->get_input_conversion(), "pack", 4) == 0) {
        // If the input takes packed, anyways, don't go through the
        // unpacker. Note that if we need the unpacking to reverse,
        // we won't get that here and might have to correct for it in
        // the decoder.
        // d_bits_in > bytes_in, so we're abusing the existence of
        // this allocated memory here
        memcpy(d_bits_in, bytes_in, nbytes_in*sizeof(uint8_t));
      }
      else {
        // Encoder takes a stream of bits, but PDU's are received as
        // bytes, so we unpack them here.
        if(d_rev_unpack)
          d_unpack->unpack_rev(d_bits_in, bytes_in, nbytes_in);
        else
          d_unpack->unpack(d_bits_in, bytes_in, nbytes_in);
      }

      // buffers for output bytes to go to
      pmt::pmt_t outvec = pmt::make_u8vector(nbytes_out, 0x00);
      uint8_t* bytes_out = pmt::u8vector_writable_elements(outvec, o0);

      // ENCODE!
      d_encoder->generic_work((void*)d_bits_in, (void*)d_bits_out);

      if(d_rev_pack)
        d_pack->pack_rev(bytes_out, d_bits_out, nbytes_out);
      else
        d_pack->pack(bytes_out, d_bits_out, nbytes_out);

      pmt::pmt_t msg_pair = pmt::cons(meta, outvec);
      message_port_pub(d_out_port, msg_pair);
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
