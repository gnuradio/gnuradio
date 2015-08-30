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

#ifndef INCLUDED_REPEAT_IMPL_H
#define INCLUDED_REPEAT_IMPL_H

#include <gnuradio/blocks/repeat.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API repeat_impl : public repeat
    {
      size_t d_itemsize;
      int d_interp;

    public:
      repeat_impl(size_t itemsize, int d_interp);

      int interpolation() const { return d_interp; }
      void set_interpolation(int interp);


      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    private:
      void msg_set_interpolation(pmt::pmt_t msg);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_REPEAT_IMPL_H */
