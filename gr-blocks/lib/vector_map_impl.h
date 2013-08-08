/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_VECTOR_MAP_IMPL_H
#define INCLUDED_GR_VECTOR_MAP_IMPL_H

#include <gnuradio/blocks/vector_map.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace blocks {

    class vector_map_impl : public vector_map
    {
    private:
      size_t d_item_size;
      std::vector<size_t> d_in_vlens;
      std::vector< std::vector< std::vector<size_t> > > d_mapping;
      gr::thread::mutex d_mutex; // mutex to protect set/work access

    public:
      vector_map_impl(size_t item_size, std::vector<size_t> in_vlens,
                      std::vector< std::vector< std::vector<size_t> > > mapping);
      ~vector_map_impl();

      void set_mapping(std::vector< std::vector< std::vector<size_t> > > mapping);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_VECTOR_MAP_IMPL_H */
