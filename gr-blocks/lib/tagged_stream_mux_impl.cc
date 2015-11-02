/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
#include "tagged_stream_mux_impl.h"

namespace gr {
  namespace blocks {

    tagged_stream_mux::sptr
    tagged_stream_mux::make(size_t itemsize, const std::string &lengthtagname,
                            unsigned int tag_preserve_head_pos)
    {
      return gnuradio::get_initial_sptr
        (new tagged_stream_mux_impl(itemsize, lengthtagname,
                                    tag_preserve_head_pos));
    }

    tagged_stream_mux_impl::tagged_stream_mux_impl(size_t itemsize, const std::string &lengthtagname,
                                                   unsigned int tag_preserve_head_pos)
      : tagged_stream_block("tagged_stream_mux",
                            io_signature::make(1, -1, itemsize),
                            io_signature::make(1,  1, itemsize),
                            lengthtagname),
        d_itemsize(itemsize),
	d_tag_preserve_head_pos(tag_preserve_head_pos)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    tagged_stream_mux_impl::~tagged_stream_mux_impl()
    {
    }

    int
    tagged_stream_mux_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int nout = 0;
      for (unsigned i = 0; i < ninput_items.size(); i++) {
	nout += ninput_items[i];
      }
      return nout;
    }

    int
    tagged_stream_mux_impl::work(int noutput_items,
                                 gr_vector_int &ninput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
      unsigned char *out = (unsigned char *) output_items[0];
      int n_produced = 0;

      set_relative_rate(ninput_items.size());

      for (unsigned int i = 0; i < input_items.size(); i++) {
	const unsigned char *in = (const unsigned char *) input_items[i];

	std::vector<tag_t> tags;
	get_tags_in_range(tags, i, nitems_read(i), nitems_read(i)+ninput_items[i]);
	for (unsigned int j = 0; j < tags.size(); j++) {
	  uint64_t offset = tags[j].offset - nitems_read(i) + nitems_written(0) + n_produced;
	  if (i == d_tag_preserve_head_pos && tags[j].offset == nitems_read(i)) {
	    offset -= n_produced;
	  }
	  add_item_tag(0, offset, tags[j].key, tags[j].value);
	}
	memcpy((void *) out, (const void *) in, ninput_items[i] * d_itemsize);
	out += ninput_items[i] * d_itemsize;
	n_produced += ninput_items[i];
      }

      return n_produced;
    }

  } /* namespace blocks */
} /* namespace gr */
