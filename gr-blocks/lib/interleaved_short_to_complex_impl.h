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

#ifndef INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H
#define INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H

#include <gnuradio/blocks/interleaved_short_to_complex.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API interleaved_short_to_complex_impl : public interleaved_short_to_complex
    {
    private:
      bool d_vector_input;
      bool d_swap;
    public:
      interleaved_short_to_complex_impl(bool vector_input=false, bool swap=false);

      void set_swap(bool swap);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */


#endif /* INCLUDED_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H */
