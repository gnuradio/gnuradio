/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PATTERNED_INTERLEAVER_IMPL_H
#define INCLUDED_PATTERNED_INTERLEAVER_IMPL_H

#include <gnuradio/blocks/patterned_interleaver.h>
#include <boost/foreach.hpp>

namespace gr {
  namespace blocks {

    class BLOCKS_API patterned_interleaver_impl : public patterned_interleaver
    {
    public:
      patterned_interleaver_impl(size_t itemsize, std::vector<int> pattern);

      int general_work(int noutput_items,
	       gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      int pattern_max(std::vector<int> pattern){
        int mval(0);
        BOOST_FOREACH( int i, pattern)
            { mval = std::max(mval, i); }
        return mval;
        }

      void forecast (int noutput_items,
             gr_vector_int &ninput_items_required);

      std::vector<int> d_pattern;
      std::vector<int> d_counts;
      size_t d_itemsize;

    };

  } /* namespace blocks */
} /* namespace gr */

#endif
