/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013 Free Software Foundation, Inc.
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

#include "message_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>

namespace gr {
  namespace blocks {

    message_sink::sptr
    message_sink::make(size_t itemsize, msg_queue::sptr msgq, bool dont_block)
    {
      return gnuradio::get_initial_sptr
        (new message_sink_impl(itemsize, msgq, dont_block));
    }

    message_sink::sptr
    message_sink::make(size_t itemsize, msg_queue::sptr msgq, bool dont_block,
		       const std::string& lengthtagname)
    {
      return gnuradio::get_initial_sptr
        (new message_sink_impl(itemsize, msgq, dont_block, lengthtagname));
    }

    message_sink_impl::message_sink_impl(size_t itemsize, msg_queue::sptr msgq, bool dont_block)
      : sync_block("message_sink",
                      io_signature::make(1, 1, itemsize),
                      io_signature::make(0, 0, 0)),
        d_itemsize(itemsize), d_msgq(msgq), d_dont_block(dont_block),
	d_tags(false), d_items_read(0)
    {
    }

    message_sink_impl::message_sink_impl(size_t itemsize, msg_queue::sptr msgq, bool dont_block,
					 const std::string& lengthtagname)
      : sync_block("message_sink",
                      io_signature::make(1, 1, itemsize),
                      io_signature::make(0, 0, 0)),
        d_itemsize(itemsize), d_msgq(msgq), d_dont_block(dont_block),
	d_tags(true), d_lengthtagname(lengthtagname), d_items_read(0)
    {
    }

    message_sink_impl::~message_sink_impl()
    {
    }

    int
    message_sink_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];

      if (d_tags) {
	long packet_length = 0;
	std::vector<tag_t> tags;
	this->get_tags_in_range(tags, 0, d_items_read, d_items_read+1);
	//const size_t ninput_items = noutput_items; //assumption for sync block, this can change
	for (unsigned int i = 0; i < tags.size(); i++) {
	  if (pmt::symbol_to_string(tags[i].key) == d_lengthtagname) {
	    packet_length = pmt::to_long(tags[i].value);
	  }
	}
	assert(packet_length != 0);

	// FIXME run this multiple times if input_items >= N * packet_length
	if (noutput_items >= packet_length ) {
	  // If the message queue is full we drop the packet.
	  if (!d_msgq->full_p()) {
            message::sptr msg = message::make(0, 		// msg type
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
          message::sptr msg = message::make(0, 		// msg type
                                            d_itemsize, 	// arg1 for other end
                                            noutput_items, 	// arg2 for other end (redundant)
                                            noutput_items * d_itemsize);   // len of msg
	  memcpy(msg->msg(), in, noutput_items * d_itemsize);

	  d_msgq->handle(msg);		// send it
	}

	return noutput_items;
      }
    }

  } /* namespace blocks */
} /* namespace gr */

