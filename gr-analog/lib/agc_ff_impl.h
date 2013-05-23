/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_AGC_FF_IMPL_H
#define INCLUDED_ANALOG_AGC_FF_IMPL_H

#include <gnuradio/analog/agc_ff.h>

namespace gr {
  namespace analog {

    class agc_ff_impl : public agc_ff, kernel::agc_ff
    {
    public:
      agc_ff_impl(float rate = 1e-4, float reference = 1.0,
		  float gain = 1.0);
      ~agc_ff_impl();

      float rate() const { return kernel::agc_ff::rate(); }
      float reference() const { return kernel::agc_ff::reference(); }
      float gain() const { return kernel::agc_ff::gain(); }
      float max_gain() const { return kernel::agc_ff::max_gain(); }

      void set_rate(float rate) { kernel::agc_ff::set_rate(rate); }
      void set_reference(float reference) { kernel::agc_ff::set_reference(reference); }
      void set_gain(float gain) { kernel::agc_ff::set_gain(gain); }
      void set_max_gain(float max_gain) { kernel::agc_ff::set_max_gain(max_gain); }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC_FF_IMPL_H */
