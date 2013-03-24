/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_message_sink.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>

// public constructor that returns a shared_ptr

gr_message_sink_sptr
gr_make_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block)
{
  return gnuradio::get_initial_sptr(new gr_message_sink(itemsize, msgq, dont_block));
}
gr_message_sink_sptr
gr_make_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block, const std::string& lengthtagname)
{
  return gnuradio::get_initial_sptr(new gr_message_sink(itemsize, msgq, dont_block, lengthtagname));
}

gr_message_sink::gr_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block)
  : gr_sync_block("message_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    d_itemsize(itemsize), d_msgq(msgq), d_dont_block(dont_block), d_tags(false), d_items_read(0)
{
}

gr_message_sink::gr_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block, const std::string& lengthtagname)
  : gr_sync_block("message_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    d_itemsize(itemsize), d_msgq(msgq), d_dont_block(dont_block), d_tags(true), d_lengthtagname(lengthtagname), d_items_read(0)
{
}

gr_message_sink::~gr_message_sink()
{
}

int
gr_message_sink::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  const char *in = (const char *) input_items[0];

  if (d_tags) {
    long packet_length = 0;
    std::vector<gr_tag_t> tags;
    this->get_tags_in_range(tags, 0, d_items_read, d_items_read+1);
    //const size_t ninput_items = noutput_items; //assumption for sync block, this can change
    for (unsigned int i = 0; i < tags.size(); i++) {
      if (pmt::pmt_symbol_to_string(tags[i].key) == d_lengthtagname) {
        packet_length = pmt::pmt_to_long(tags[i].value);
      }
    }
    assert(packet_length != 0);

	// FIXME run this multiple times if input_items >= N * packet_length
	if (noutput_items >= packet_length ) {
      // If the message queue is full we drop the packet.
      if (!d_msgq->full_p()) {
        gr_message_sptr msg = gr_make_message(0, 		// msg type
                                              d_itemsize, 	// arg1 for other end
                                              packet_length, 	// arg2 for other end (redundant)
                                              packet_length * d_itemsize);   // len of msg
        memcpy(msg->msg(), in, packet_length * d_itemsize);
        d_msgq->handle(msg);		// send it
      }
      d_items_read += packet_length;
      return packet_length;
    } else {
      return 0;
    }
  } else {
    // If the queue if full we drop all the data we got.
    if (!d_msgq->full_p()) {
      // build a message to hold whatever we've got
      gr_message_sptr msg = gr_make_message(0, 		// msg type
                                            d_itemsize, 	// arg1 for other end
                                            noutput_items, 	// arg2 for other end (redundant)
                                            noutput_items * d_itemsize);   // len of msg
      memcpy(msg->msg(), in, noutput_items * d_itemsize);
      
      d_msgq->handle(msg);		// send it
    }
    return noutput_items;
  }
}
