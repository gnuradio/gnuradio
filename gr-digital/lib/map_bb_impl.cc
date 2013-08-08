/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2010,2012 Free Software Foundation, Inc.
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

#include "map_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    map_bb::sptr
    map_bb::make(const std::vector<int> &map)
    {
      return gnuradio::get_initial_sptr(new map_bb_impl(map));
    }

    map_bb_impl::map_bb_impl(const std::vector<int> &map)
      : sync_block("map_bb",
		      io_signature::make(1, 1, sizeof(unsigned char)),
		      io_signature::make(1, 1, sizeof(unsigned char)))
    {
      set_map(map);
    }

    map_bb_impl::~map_bb_impl()
    {
    }

    void
    map_bb_impl::set_map(const std::vector<int> &map)
    {
      gr::thread::scoped_lock guard(d_mutex);

      for(int i = 0; i < 0x100; i++)
	d_map[i] = i;

      unsigned int size = std::min((size_t)0x100, map.size());
      for(unsigned int i = 0; i < size; i++)
	d_map[i] = map[i];
    }

    std::vector<int>
    map_bb_impl::map() const
    {
      std::vector<int> m;
      for(unsigned  i = 0; i < 0x100; i++)
	m[i] = d_map[i];
      return m;
    }

    int
    map_bb_impl::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      const unsigned char *in = (const unsigned char*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      for(int i = 0; i < noutput_items; i++)
	out[i] = d_map[in[i]];

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
