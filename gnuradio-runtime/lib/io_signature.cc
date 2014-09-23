/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2014 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <iostream>

namespace gr {

  gr::io_signature::sptr
  io_signature::makev(int min_streams, int max_streams,
                      const std::vector<int> &sizeof_stream_items,
		      const std::vector<uint32_t> &stream_flags)
  {
    return gr::io_signature::sptr
      (new io_signature(min_streams, max_streams,
                        sizeof_stream_items, stream_flags));
  }

  gr::io_signature::sptr
  io_signature::make(int min_streams, int max_streams,
                     int sizeof_stream_item,
		     uint32_t flags)
  {
    std::vector<int> sizeof_items(1);
    std::vector<uint32_t> stream_flags(1);
    sizeof_items[0] = sizeof_stream_item;
    stream_flags[0] = flags;
    return io_signature::makev(min_streams, max_streams, sizeof_items, stream_flags);
  }

  gr::io_signature::sptr
  io_signature::make2(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
		      uint32_t flags1,
		      uint32_t flags2)
  {
    std::vector<int> sizeof_items(2);
    std::vector<uint32_t> stream_flags(2);
    sizeof_items[0] = sizeof_stream_item1;
    sizeof_items[1] = sizeof_stream_item2;
    stream_flags[0] = flags1;
    stream_flags[1] = flags2;
    return io_signature::makev(min_streams, max_streams, sizeof_items, stream_flags);
  }

  gr::io_signature::sptr
  io_signature::make3(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3,
		      uint32_t flags1,
		      uint32_t flags2,
		      uint32_t flags3)
  {
    std::vector<int> sizeof_items(3);
    std::vector<uint32_t> stream_flags(3);
    sizeof_items[0] = sizeof_stream_item1;
    sizeof_items[1] = sizeof_stream_item2;
    sizeof_items[2] = sizeof_stream_item3;
    stream_flags[0] = flags1;
    stream_flags[1] = flags2;
    stream_flags[2] = flags3;
    return io_signature::makev(min_streams, max_streams, sizeof_items, stream_flags);
  }

  // ------------------------------------------------------------------------

  io_signature::io_signature(int min_streams, int max_streams,
                             const std::vector<int> &sizeof_stream_items,
			     const std::vector<uint32_t> &stream_flags)
  {
    if(min_streams < 0
       || (max_streams != IO_INFINITE && max_streams < min_streams))
      throw std::invalid_argument("gr::io_signature(1)");

    if(sizeof_stream_items.size() < 1)
      throw std::invalid_argument("gr::io_signature(2)");

    for(size_t i = 0; i < sizeof_stream_items.size(); i++) {
      if(max_streams != 0 && sizeof_stream_items[i] < 1)
        throw std::invalid_argument("gr::io_signature(3)");
    }

    d_min_streams = min_streams;
    d_max_streams = max_streams;
    d_sizeof_stream_item = sizeof_stream_items;

    // If user doesn't supply stream flags, make vector of defaults
    // of the same size as sizeof_stream_items
    if(stream_flags.size() == 0) {
      for (size_t i = 0; i < sizeof_stream_items.size(); i++)
	d_stream_flags.push_back(io_signature::DEFAULT_FLAGS);
    }
    else {
      // Make sure user supplied vector matches
      if(sizeof_stream_items.size() != stream_flags.size())
	throw std::invalid_argument("gr::io_signature(4)");

      d_stream_flags = stream_flags;
    }
  }

  io_signature::~io_signature()
  {
  }

  int
  io_signature::sizeof_stream_item(int _index) const
  {
    if(_index < 0)
      throw std::invalid_argument("gr::io_signature::sizeof_stream_item");

    size_t index = _index;
    return d_sizeof_stream_item[std::min(index, d_sizeof_stream_item.size() - 1)];
  }

  uint32_t
  io_signature::stream_flags(int _index) const
  {
    if(_index < 0)
      throw std::invalid_argument("gr::io_signature::sizeof_stream_item");

    size_t index = _index;
    return d_stream_flags[std::min(index, d_stream_flags.size() - 1)];
  }

  std::vector<int>
  io_signature::sizeof_stream_items() const
  {
    return d_sizeof_stream_item;
  }

  std::vector<uint32_t>
  io_signature::stream_flags() const
  {
    return d_stream_flags;
  }

} /* namespace gr */
