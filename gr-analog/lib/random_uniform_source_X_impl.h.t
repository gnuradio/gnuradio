/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/analog/@BASE_NAME@.h>
#include <gnuradio/random.h>

namespace gr {
  namespace analog {

    class @IMPL_NAME@ : public @BASE_NAME@
    {
    private:
      gr::random *d_rng;

    public:
      @IMPL_NAME@(int minimum, int maximum, int seed);
      ~@IMPL_NAME@();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      int random_value();
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* @GUARD_NAME@ */