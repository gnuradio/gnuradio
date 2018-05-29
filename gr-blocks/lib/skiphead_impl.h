/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SKIPHEAD_IMPL_H
#define INCLUDED_GR_SKIPHEAD_IMPL_H

#include <gnuradio/blocks/skiphead.h>

namespace gr {
  namespace blocks {

    class skiphead_impl : public skiphead
    {
    private:
      uint64_t d_nitems_to_skip;
      uint64_t d_nitems;           // total items seen
      std::vector<tag_t> d_tags;

    public:
      skiphead_impl(size_t itemsize, uint64_t nitems_to_skip);
      ~skiphead_impl();

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SKIPHEAD_IMPL_H */
