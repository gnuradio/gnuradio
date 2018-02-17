/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2017-2018 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <@NAME_IMPL@.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <algorithm>
#include <iostream>

namespace gr {
  namespace blocks {

    @NAME@::sptr
    @BASE_NAME@::make(const int vlen, const int reserve_items)
    {
      return gnuradio::get_initial_sptr
        (new @NAME_IMPL@(vlen, reserve_items));
    }

    @NAME_IMPL@::@NAME_IMPL@(const int vlen, const int reserve_items)
    : sync_block("@NAME@",
                    io_signature::make(1, 1, sizeof(@TYPE@) * vlen),
                    io_signature::make(0, 0, 0)),
    d_vlen(vlen)
    {
      gr::thread::scoped_lock guard(d_data_mutex);
      d_data.reserve(d_vlen * reserve_items);
    }

    @NAME_IMPL@::~@NAME_IMPL@()
    {}

    std::vector<@TYPE@>
    @NAME_IMPL@::data() const
    {
      gr::thread::scoped_lock guard(d_data_mutex);
      return d_data;
    }

    std::vector<tag_t>
    @NAME_IMPL@::tags() const
    {
      gr::thread::scoped_lock guard(d_data_mutex);
      return d_tags;
    }


    void
    @NAME_IMPL@::reset()
    {
      gr::thread::scoped_lock guard(d_data_mutex);
      d_tags.clear();
      d_data.clear();
    }

    int
    @NAME_IMPL@::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      @TYPE@ *iptr = (@TYPE@*)input_items[0];

      // can't touch this (as long as work() is working, the accessors shall not
      // read the data
      gr::thread::scoped_lock guard(d_data_mutex);
      for(int i = 0; i < noutput_items * d_vlen; i++)
        d_data.push_back (iptr[i]);
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
      d_tags.insert(d_tags.end(), tags.begin(), tags.end());
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
