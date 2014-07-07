/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "@NAME_IMPL@.h"

namespace gr {
  namespace blocks {

    @NAME@::sptr
    @NAME@::make(int vlen, const std::string &tsb_key)
    {
      return gnuradio::get_initial_sptr
        (new @NAME_IMPL@(vlen, tsb_key));
    }

    @NAME_IMPL@::@NAME_IMPL@(int vlen, const std::string &tsb_key)
      : gr::tagged_stream_block("@NAME@",
              gr::io_signature::make(1, 1, vlen * sizeof(@TYPE@)),
              gr::io_signature::make(0, 0, 0), tsb_key),
      d_vlen(vlen)
    {}

    @NAME_IMPL@::~@NAME_IMPL@()
    {
    }

    std::vector<std::vector<@TYPE@> >
    @NAME_IMPL@::data() const
    {
      return d_data;
    }

    std::vector<tag_t>
    @NAME_IMPL@::tags() const
    {
      return d_tags;
    }

    int
    @NAME_IMPL@::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const @TYPE@ *in = (const @TYPE@ *) input_items[0];

      std::vector<@TYPE@> new_data(in, in + (ninput_items[0] * d_vlen));
      d_data.push_back(new_data);

      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, 0, ninput_items[0]);
      d_tags.insert(d_tags.end(), tags.begin(), tags.end());

      return ninput_items[0];
    }

  } /* namespace blocks */
} /* namespace gr */

