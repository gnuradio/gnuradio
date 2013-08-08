/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include "packet_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <gnuradio/blocks/count_bits.h>
#include <string.h>

namespace gr {
  namespace digital {

#define VERBOSE 0

// detect access code with up to DEFAULT_THRESHOLD bits wrong
static const int DEFAULT_THRESHOLD = 12;

    inline void
    packet_sink_impl::enter_search()
    {
      if(VERBOSE)
	fprintf(stderr, "@ enter_search\n");

      d_state = STATE_SYNC_SEARCH;
      d_shift_reg = 0;
    }

    inline void
    packet_sink_impl::enter_have_sync()
    {
      if(VERBOSE)
	fprintf(stderr, "@ enter_have_sync\n");

      d_state = STATE_HAVE_SYNC;
      d_header = 0;
      d_headerbitlen_cnt = 0;
    }

    inline void
    packet_sink_impl::enter_have_header(int payload_len)
    {
      if(VERBOSE)
	fprintf(stderr, "@ enter_have_header (payload_len = %d)\n", payload_len);

      d_state = STATE_HAVE_HEADER;
      d_packetlen  = payload_len;
      d_packetlen_cnt = 0;
      d_packet_byte = 0;
      d_packet_byte_index = 0;
    }

    packet_sink::sptr
    packet_sink::make(const std::vector<unsigned char>& sync_vector,
		      msg_queue::sptr target_queue, int threshold)
    {
      return gnuradio::get_initial_sptr
	(new packet_sink_impl(sync_vector, target_queue, threshold));
    }

    packet_sink_impl::packet_sink_impl(const std::vector<unsigned char>& sync_vector,
				       msg_queue::sptr target_queue, int threshold)
      : sync_block("packet_sink",
		      io_signature::make(1, 1, sizeof(float)),
		      io_signature::make(0, 0, 0)),
	d_target_queue(target_queue), d_threshold(threshold == -1 ? DEFAULT_THRESHOLD : threshold)
    {
      d_sync_vector = 0;
      for(int i=0;i<8;i++){
	d_sync_vector <<= 8;
	d_sync_vector |= sync_vector[i];
      }

      enter_search();
    }

    packet_sink_impl::~packet_sink_impl()
    {
    }

    int
    packet_sink_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      float *inbuf = (float*)input_items[0];
      int count=0;

      if(VERBOSE)
	fprintf(stderr, ">>> Entering state machine\n"), fflush(stderr);

      while(count<noutput_items) {
	switch(d_state) {

	case STATE_SYNC_SEARCH:    // Look for sync vector
	  if(VERBOSE)
	    fprintf(stderr, "SYNC Search, noutput=%d\n",noutput_items), fflush(stderr);

	  while(count < noutput_items) {
	    if(slice(inbuf[count++]))
	      d_shift_reg = (d_shift_reg << 1) | 1;
	    else
	      d_shift_reg = d_shift_reg << 1;

	    // Compute popcnt of putative sync vector
	    if(gr::blocks::count_bits64(d_shift_reg ^ d_sync_vector) <= d_threshold) {
	      // Found it, set up for header decode
	      enter_have_sync();
	      break;
	    }
	  }
	  break;

	case STATE_HAVE_SYNC:
	  if(VERBOSE)
	    fprintf(stderr, "Header Search bitcnt=%d, header=0x%08x\n", d_headerbitlen_cnt, d_header),
	      fflush(stderr);

	  while(count < noutput_items) {	// Shift bits one at a time into header
	    if(slice(inbuf[count++]))
	      d_header = (d_header << 1) | 1;
	    else
	      d_header = d_header << 1;

	    if(++d_headerbitlen_cnt == HEADERBITLEN) {
	      if(VERBOSE)
		fprintf(stderr, "got header: 0x%08x\n", d_header);

	      // we have a full header, check to see if it has been received properly
	      if(header_ok()) {
		int payload_len = header_payload_len();
		if(payload_len <= MAX_PKT_LEN)		// reasonable?
		  enter_have_header(payload_len);	// yes.
		else
		  enter_search();			// no.
	      }
	      else
		enter_search();				// no.
	      break;			// we're in a new state
	    }
	  }
	  break;

	case STATE_HAVE_HEADER:
	  if(VERBOSE)
	    fprintf(stderr,"Packet Build\n"),fflush(stderr);

	  while(count < noutput_items) {   // shift bits into bytes of packet one at a time
	    if(slice(inbuf[count++]))
	      d_packet_byte = (d_packet_byte << 1) | 1;
	    else
	      d_packet_byte = d_packet_byte << 1;

	    if(d_packet_byte_index++ == 7) {	  	// byte is full so move to next byte
	      d_packet[d_packetlen_cnt++] = d_packet_byte;
	      d_packet_byte_index = 0;

	      if(d_packetlen_cnt == d_packetlen) {		// packet is filled
		// build a message
		message::sptr msg = message::make(0, 0, 0, d_packetlen_cnt);
		memcpy(msg->msg(), d_packet, d_packetlen_cnt);

		d_target_queue->insert_tail(msg);	// send it
		msg.reset();  				// free it up

		enter_search();
		break;
	      }
	    }
	  }
	  break;

	default:
	  assert(0);
	} // switch
      }   // while

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
