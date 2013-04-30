/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H
#define INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H

#include <gnuradio/digital/glfsr_source_b.h>
#include <gnuradio/digital/glfsr.h>

namespace gr {
  namespace digital {

    class glfsr_source_b_impl : public glfsr_source_b
    {
    private:
      glfsr *d_glfsr;

      bool d_repeat;
      unsigned int d_index;
      unsigned int d_length;

    public:
      glfsr_source_b_impl(int degree, bool repeat=true,
			  int mask=0, int seed=1);
      ~glfsr_source_b_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      unsigned int period() const { return d_length; }
      int mask() const;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_GLFSR_SOURCE_B_IMPL_H */
