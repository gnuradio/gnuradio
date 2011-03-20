/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <gr_nop.h>
#include <gr_io_signature.h>
#include <boost/bind.hpp>

gr_nop_sptr
gr_make_nop (size_t sizeof_stream_item)
{
  return gnuradio::get_initial_sptr (new gr_nop (sizeof_stream_item));
}

gr_nop::gr_nop (size_t sizeof_stream_item)
  : gr_block ("nop",
	      gr_make_io_signature (0, -1, sizeof_stream_item),
	      gr_make_io_signature (0, -1, sizeof_stream_item)),
    d_nmsgs_recvd(0)
{
  // Arrange to have count_received_msgs called when messages are received.
  set_msg_handler(boost::bind(&gr_nop::count_received_msgs, this, _1));
}

// Trivial message handler that just counts them.
// (N.B., This feature is used in qa_set_msg_handler)
void
gr_nop::count_received_msgs(pmt::pmt_t msg)
{
  d_nmsgs_recvd++;
}

int
gr_nop::general_work (int noutput_items,
		      gr_vector_int &ninput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  // eat any input that's available
  for (unsigned i = 0; i < ninput_items.size (); i++)
    consume (i, ninput_items[i]);
  
  return noutput_items;
}
