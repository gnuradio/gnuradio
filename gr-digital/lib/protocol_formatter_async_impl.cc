/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
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

#include <volk/volk.h>
#include <gnuradio/io_signature.h>
#include "protocol_formatter_async_impl.h"
#include <stdio.h>

namespace gr {
  namespace digital {

    protocol_formatter_async::sptr
    protocol_formatter_async::make(const header_format_base::sptr &format)
    {
      return gnuradio::get_initial_sptr
        (new protocol_formatter_async_impl(format));
    }

    protocol_formatter_async_impl::protocol_formatter_async_impl(const header_format_base::sptr &format)
      : block("protocol_formatter_async",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0))
    {
      d_format = format;

      d_in_port = pmt::mp("in");
      d_hdr_port = pmt::mp("header");
      d_pld_port = pmt::mp("payload");

      message_port_register_in(d_in_port);
      message_port_register_out(d_hdr_port);
      message_port_register_out(d_pld_port);

      set_msg_handler(d_in_port,
                      boost::bind(&protocol_formatter_async_impl::append, this ,_1) );
    }

    protocol_formatter_async_impl::~protocol_formatter_async_impl()
    {
    }

    void
    protocol_formatter_async_impl::append(pmt::pmt_t msg)
    {
      // extract input pdu
      pmt::pmt_t meta(pmt::car(msg));
      pmt::pmt_t input(pmt::cdr(msg));
      pmt::pmt_t header, output;

      size_t pkt_len = 0;
      const uint8_t* bytes_in = pmt::u8vector_elements(input, pkt_len);

      // Pad the payload with 0's
      uint8_t* payload = (uint8_t*)volk_malloc(pkt_len*sizeof(uint8_t),
                                               volk_get_alignment());
      memcpy(payload, bytes_in, pkt_len*sizeof(uint8_t));
      output = pmt::init_u8vector(pkt_len, payload);
      volk_free(payload);

      // Build the header from the input, metadata, and format
      d_format->format(pkt_len, bytes_in, header, meta);

      // Package and publish
      pmt::pmt_t hdr_pdu = pmt::cons(meta, header);
      pmt::pmt_t pld_pdu = pmt::cons(meta, output);
      message_port_pub(d_hdr_port, hdr_pdu);
      message_port_pub(d_pld_port, pld_pdu);
    }

  } /* namespace digital */
} /* namespace gr */
