/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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
#include "tag_share_impl.h"

namespace gr {
  namespace blocks {

    tag_share::sptr
    tag_share::make(size_t sizeof_io_item, size_t sizeof_share_item, size_t vlen)
    {
      return gnuradio::get_initial_sptr
        (new tag_share_impl(sizeof_io_item, sizeof_share_item, vlen));
    }

    /*
     * The private constructor
     */
    tag_share_impl::tag_share_impl(size_t sizeof_io_item, size_t sizeof_share_item, size_t vlen)
      : gr::sync_block("tag_share",
              gr::io_signature::make2(2, 2, sizeof_io_item*vlen, sizeof_share_item*vlen),
              gr::io_signature::make(1, 1, sizeof_io_item*vlen)),
        d_sizeof_io_item(sizeof_io_item),
        d_vlen(vlen)
    {
      // This is the entire premise of the block -- to have the GNU Radio runtime
      // propagate all the tags from Input 0 and Input 1 to Output 0.
      set_tag_propagation_policy(TPP_ALL_TO_ALL);
    }

    /*
     * Our virtual destructor.
     */
    tag_share_impl::~tag_share_impl()
    {
    }

    int
    tag_share_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const void *in = (const void *) input_items[0];
      void *out = (void *) output_items[0];

      // Input 0 passes through to Output 0
      memcpy(out, in, d_sizeof_io_item*d_vlen*noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

