/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2015 Free Software Foundation, Inc.
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

#ifndef ADD_CONST_BB_IMPL
#define ADD_CONST_BB_IMPL

#include <gnuradio/blocks/add_const_bb.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API add_const_bb_impl : public add_const_bb
    {
    private:
      unsigned char d_k;

    public:
      add_const_bb_impl(unsigned char k);
      ~add_const_bb_impl();

      void setup_rpc();

      unsigned char k() const { return d_k; }
      void set_k(unsigned char k) { d_k = k; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* ADD_CONST_BB_IMPL */
