/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H
#define INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H

#include <gnuradio/analog/quadrature_demod_cf.h>

namespace gr {
  namespace analog {

    class quadrature_demod_cf_impl : public quadrature_demod_cf
    {
    private:
      float d_gain;

    public:
      quadrature_demod_cf_impl(float gain);
      ~quadrature_demod_cf_impl();

      void set_gain(float gain) { d_gain = gain; }
      float gain() const { return d_gain; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H */
