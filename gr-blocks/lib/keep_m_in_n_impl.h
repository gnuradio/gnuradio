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

#ifndef INCLUDED_KEEP_M_IN_N_IMPL_H
#define INCLUDED_KEEP_M_IN_N_IMPL_H

#include <gnuradio/blocks/keep_m_in_n.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API keep_m_in_n_impl : public keep_m_in_n
    {
      int d_m;
      int d_n;
      int d_offset;
      int d_itemsize;

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

    public:
      keep_m_in_n_impl(size_t itemsize, int m, int n, int offset);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      void set_m(int m);
      void set_n(int n);
      void set_offset(int offset);
    };

  } /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_KEEP_M_IN_N_IMPL_H */
