/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FRAMER_SINK_1_IMPL_H
#define INCLUDED_GR_FRAMER_SINK_1_IMPL_H

#include <gnuradio/digital/framer_sink_1.h>

namespace gr {
  namespace digital {

    class framer_sink_1_impl : public framer_sink_1
    {
    private:
      enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER};

      static const int MAX_PKT_LEN    = 4096;
      static const int HEADERBITLEN   = 32;

      msg_queue::sptr  d_target_queue;	    // where to send the packet when received
      state_t          d_state;
      unsigned int     d_header;	      // header bits
      int	       d_headerbitlen_cnt;    // how many so far

      unsigned char    d_packet[MAX_PKT_LEN]; // assembled payload
      unsigned char    d_packet_byte;	      // byte being assembled
      int	       d_packet_byte_index;   // which bit of d_packet_byte we're working on
      int	       d_packetlen;		   // length of packet
      int              d_packet_whitener_offset;   // offset into whitener string to use
      int	       d_packetlen_cnt;	           // how many so far

    protected:
      void enter_search();
      void enter_have_sync();
      void enter_have_header(int payload_len, int whitener_offset);

      bool header_ok()
      {
	// confirm that two copies of header info are identical
	return ((d_header >> 16) ^ (d_header & 0xffff)) == 0;
      }

      void header_payload(int *len, int *offset)
      {
	// header consists of two 16-bit shorts in network byte order
	// payload length is lower 12 bits
	// whitener offset is upper 4 bits
	*len = (d_header >> 16) & 0x0fff;
	*offset = (d_header >> 28) & 0x000f;
      }

    public:
      framer_sink_1_impl(msg_queue::sptr target_queue);
      ~framer_sink_1_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_FRAMER_SINK_1_IMPL_H */
