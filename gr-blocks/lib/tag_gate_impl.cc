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

#include <gnuradio/io_signature.h>
#include "tag_gate_impl.h"

namespace gr {
  namespace blocks {

    tag_gate::sptr
    tag_gate::make(size_t item_size, bool propagate_tags)
    {
      return gnuradio::get_initial_sptr (new tag_gate_impl(item_size, propagate_tags));
    }

    tag_gate_impl::tag_gate_impl(size_t item_size, bool propagate_tags)
      : gr::sync_block("tag_gate",
		       gr::io_signature::make(1, 1, item_size),
		       gr::io_signature::make(1, 1, item_size)),
      d_item_size(item_size),
      d_propagate_tags(propagate_tags)
    {
      if (!d_propagate_tags) {
	set_tag_propagation_policy(TPP_DONT);
      }
    }

    tag_gate_impl::~tag_gate_impl()
    {
    }

    void tag_gate_impl::set_propagation(bool propagate_tags)
    {
      if (propagate_tags) {
	set_tag_propagation_policy(TPP_ALL_TO_ALL);
      } else {
	set_tag_propagation_policy(TPP_DONT);
      }
    }

    int
    tag_gate_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

	memcpy((void *) out, (void *) in, d_item_size * noutput_items);

        return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

