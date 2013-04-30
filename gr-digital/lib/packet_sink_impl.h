/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PACKET_SINK_IMPL_H
#define INCLUDED_GR_PACKET_SINK_IMPL_H

#include <gnuradio/digital/packet_sink.h>

namespace gr {
  namespace digital {

    class packet_sink_impl : public packet_sink
    {
    private:
      enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER};

      static const int MAX_PKT_LEN    = 4096;
      static const int HEADERBITLEN   = 32;

      msg_queue::sptr  d_target_queue;	// where to send the packet when received
      unsigned long long d_sync_vector;		// access code to locate start of packet
      unsigned int       d_threshold;		// how many bits may be wrong in sync vector
      
      state_t            d_state;

      unsigned long long d_shift_reg;		// used to look for sync_vector

      unsigned int       d_header;		// header bits
      int	         d_headerbitlen_cnt;// how many so far

      unsigned char      d_packet[MAX_PKT_LEN];	// assembled payload
      unsigned char      d_packet_byte;         // byte being assembled
      int                d_packet_byte_index;	// which bit of d_packet_byte we're working on
      int                d_packetlen;		// length of packet
      int                d_packetlen_cnt;	// how many so far

    protected:
      void enter_search();
      void enter_have_sync();
      void enter_have_header(int payload_len);

      int slice(float x) { return x > 0 ? 1 : 0; }

      bool header_ok()
      {
	// confirm that two copies of header info are identical
	return ((d_header >> 16) ^ (d_header & 0xffff)) == 0;
      }

      int header_payload_len()
      {
	// header consists of two 16-bit shorts in network byte order
	int t = (d_header >> 16) & 0xffff;
	return t;
      }

    public:
      packet_sink_impl(const std::vector<unsigned char>& sync_vector,
		       msg_queue::sptr target_queue,
		       int threshold=-1);
      ~packet_sink_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      //! return true if we detect carrier
      bool carrier_sensed() const
      {
	return d_state != STATE_SYNC_SEARCH;
      }
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PACKET_SINK_IMPL_H */
