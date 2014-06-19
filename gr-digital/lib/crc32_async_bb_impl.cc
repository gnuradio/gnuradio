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

#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include "crc32_async_bb_impl.h"

namespace gr {
  namespace digital {

    crc32_async_bb::sptr
    crc32_async_bb::make(bool check)
    {
      return gnuradio::get_initial_sptr
        (new crc32_async_bb_impl(check));
    }

    crc32_async_bb_impl::crc32_async_bb_impl(bool check)
      : block("crc32_async_bb",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0)),
	d_check(check), d_npass(0), d_nfail(0)
    {
      d_in_port = pmt::mp("in");
      d_out_port = pmt::mp("out");

      message_port_register_in(d_in_port);
      message_port_register_out(d_out_port);

      if(check)
        set_msg_handler(d_in_port, boost::bind(&crc32_async_bb_impl::check, this ,_1) );
      else
        set_msg_handler(d_in_port, boost::bind(&crc32_async_bb_impl::calc, this ,_1) );
    }

    crc32_async_bb_impl::~crc32_async_bb_impl()
    {
    }

    void
    crc32_async_bb_impl::calc(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bytes(pmt::cdr(msg));

      unsigned int crc;
      size_t pkt_len(0);
      const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);
      uint8_t* bytes_out = (uint8_t*)volk_malloc(4 + pkt_len*sizeof(uint8_t),
                                                 volk_get_alignment());

      d_crc_impl.reset();
      d_crc_impl.process_bytes(bytes_in, pkt_len);
      crc = d_crc_impl();
      memcpy((void*)bytes_out, (const void*)bytes_in, pkt_len);
      memcpy((void*)(bytes_out + pkt_len), &crc, 4); // FIXME big-endian/little-endian, this might be wrong

      pmt::pmt_t output = pmt::init_u8vector(pkt_len+4, bytes_out); // this copies the values from bytes_out into the u8vector
      pmt::pmt_t msg_pair = pmt::cons(meta, output);
      message_port_pub(d_out_port, msg_pair);
	  volk_free(bytes_out);
    }

    void
    crc32_async_bb_impl::check(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t bytes(pmt::cdr(msg));

      unsigned int crc;
      size_t pkt_len(0);
      const uint8_t* bytes_in = pmt::u8vector_elements(bytes, pkt_len);

      d_crc_impl.reset();
      d_crc_impl.process_bytes(bytes_in, pkt_len-4);
      crc = d_crc_impl();
      if(crc != *(unsigned int*)(bytes_in+pkt_len-4)) { // Drop package
        d_nfail++;
        return;
      }
      d_npass++;

      pmt::pmt_t output = pmt::init_u8vector(pkt_len-4, bytes_in);
      pmt::pmt_t msg_pair = pmt::cons(meta, output);
      message_port_pub(d_out_port, msg_pair);
    }

    int
    crc32_async_bb_impl::general_work(int noutput_items,
                                      gr_vector_int &ninput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
