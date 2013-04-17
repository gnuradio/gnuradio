/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
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
                      const std::vector<int> &sizeof_stream_items)
  {
    return gr::io_signature::sptr
      (new io_signature(min_streams, max_streams,
                        sizeof_stream_items));
  }

  gr::io_signature::sptr
  io_signature::make(int min_streams, int max_streams,
                     int sizeof_stream_item)
  {
    std::vector<int> sizeof_items(1);
    sizeof_items[0] = sizeof_stream_item;
    return io_signature::makev(min_streams, max_streams, sizeof_items);
  }

  gr::io_signature::sptr
  io_signature::make2(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2)
  {
    std::vector<int> sizeof_items(2);
    sizeof_items[0] = sizeof_stream_item1;
    sizeof_items[1] = sizeof_stream_item2;
    return io_signature::makev(min_streams, max_streams, sizeof_items);
  }

  gr::io_signature::sptr
  io_signature::make3(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3)
  {
    std::vector<int> sizeof_items(3);
    sizeof_items[0] = sizeof_stream_item1;
    sizeof_items[1] = sizeof_stream_item2;
    sizeof_items[2] = sizeof_stream_item3;
    return io_signature::makev(min_streams, max_streams, sizeof_items);
  }

  // ------------------------------------------------------------------------

  io_signature::io_signature(int min_streams, int max_streams,
                             const std::vector<int> &sizeof_stream_items)
  {
    if(min_streams < 0
       || (max_streams != IO_INFINITE && max_streams < min_streams))
      throw std::invalid_argument ("gr::io_signature(1)");

    if(sizeof_stream_items.size() < 1)
      throw std::invalid_argument("gr::io_signature(2)");

    for(size_t i = 0; i < sizeof_stream_items.size(); i++) {
      if(max_streams != 0 && sizeof_stream_items[i] < 1)
        throw std::invalid_argument("gr::io_signature(3)");
    }

    d_min_streams = min_streams;
    d_max_streams = max_streams;
    d_sizeof_stream_item = sizeof_stream_items;
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

  std::vector<int>
  io_signature::sizeof_stream_items() const
  {
    return d_sizeof_stream_item;
  }

} /* namespace gr */
