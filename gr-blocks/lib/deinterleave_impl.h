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

#ifndef INCLUDED_DEINTERLEAVE_IMPL_H
#define INCLUDED_DEINTERLEAVE_IMPL_H

#include <gnuradio/blocks/deinterleave.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API deinterleave_impl : public deinterleave
    {

      size_t d_itemsize;
      unsigned int d_blocksize;
      unsigned int d_current_output;
      unsigned int d_noutputs;
      unsigned int d_size_bytes; // block size in bytes

    public:
      deinterleave_impl(size_t itemsize, unsigned int blocksize);

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      bool check_topology(int ninputs, int noutputs);

      int general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

    };

  } /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_DEINTERLEAVE_IMPL_H */
