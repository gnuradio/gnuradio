/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MAP_BB_IMPL_H
#define INCLUDED_GR_MAP_BB_IMPL_H

#include <gnuradio/digital/map_bb.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace digital {

    class map_bb_impl : public map_bb
    {
    private:
      unsigned char d_map[0x100];
      gr::thread::mutex d_mutex;

    public:
      map_bb_impl(const std::vector<int> &map);
      ~map_bb_impl();

      void set_map(const std::vector<int> &map);
      std::vector<int> map() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_MAP_BB_IMPL_H */
