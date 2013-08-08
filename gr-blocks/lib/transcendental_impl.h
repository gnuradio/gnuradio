/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TRANSCENDENTAL_IMPL_H
#define INCLUDED_GR_TRANSCENDENTAL_IMPL_H

#include <gnuradio/blocks/transcendental.h>

namespace gr {
  namespace blocks {

    typedef int(*work_fcn_type)(int, gr_vector_const_void_star &, gr_vector_void_star &);

    class transcendental_impl : public transcendental
    {
    private:
      const work_fcn_type &_work_fcn;

    public:
      transcendental_impl(const work_fcn_type &work_fcn,
                          const size_t io_size);
      ~transcendental_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TRANSCENDENTAL_IMPL_H */
