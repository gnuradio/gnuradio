/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <cstring>
#include <gnuradio/io_signature.h>
#include "stream_to_tagged_stream_impl.h"

namespace gr {
  namespace blocks {

    stream_to_tagged_stream::sptr
    stream_to_tagged_stream::make(size_t itemsize, int vlen, unsigned packet_len, const std::string &len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new stream_to_tagged_stream_impl(itemsize, vlen, packet_len, len_tag_key));
    }

    stream_to_tagged_stream_impl::stream_to_tagged_stream_impl(size_t itemsize, int vlen, unsigned packet_len, const std::string &len_tag_key)
      : gr::sync_block("stream_to_tagged_stream",
              gr::io_signature::make(1, 1, itemsize * vlen),
              gr::io_signature::make(1, 1, itemsize * vlen)),
      d_itemsize(itemsize * vlen),
      d_packet_len(packet_len),
      d_packet_len_pmt(pmt::from_long(packet_len)),
      d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      d_next_tag_pos(0)
    {}

    stream_to_tagged_stream_impl::~stream_to_tagged_stream_impl()
    {
    }

    void
    stream_to_tagged_stream_impl::set_packet_len(unsigned packet_len)
    {
	gr::thread::scoped_lock guard(d_setlock);
	d_packet_len = packet_len;
    }      
    void
    stream_to_tagged_stream_impl::set_packet_len_pmt(unsigned packet_len)
    {
	gr::thread::scoped_lock guard(d_setlock);
	d_packet_len_pmt=pmt::from_long(packet_len);
    }      
    int
    stream_to_tagged_stream_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
	gr::thread::scoped_lock guard(d_setlock);
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
	// Copy data
	memcpy(out, in, noutput_items * d_itemsize);
	// Add tags every d_packet_len
	while(d_next_tag_pos < nitems_written(0) + noutput_items) {
	  add_item_tag(0, d_next_tag_pos, d_len_tag_key, d_packet_len_pmt);
	  d_next_tag_pos += d_packet_len;
	}

        return noutput_items;
    }

    bool
    stream_to_tagged_stream_impl::start()
    {
        d_next_tag_pos = 0;
        return true;
    }

  } /* namespace blocks */
} /* namespace gr */

