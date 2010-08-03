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

gr_message_sink::gr_message_sink (size_t itemsize, gr_msg_queue_sptr msgq, bool dont_block)
  : gr_sync_block("message_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    d_itemsize(itemsize), d_msgq(msgq), d_dont_block(dont_block)
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

  // if we'd block, drop the data on the floor and say everything is OK
  if (d_dont_block && d_msgq->full_p())
    return noutput_items;

  // build a message to hold whatever we've got
  gr_message_sptr msg = gr_make_message(0, 		// msg type
					d_itemsize, 	// arg1 for other end
					noutput_items, 	// arg2 for other end (redundant)
					noutput_items * d_itemsize);   // len of msg
  memcpy(msg->msg(), in, noutput_items * d_itemsize);

  d_msgq->handle(msg);		// send it

  return noutput_items;
}
