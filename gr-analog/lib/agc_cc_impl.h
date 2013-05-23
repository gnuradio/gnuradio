/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_AGC_CC_IMPL_H
#define INCLUDED_ANALOG_AGC_CC_IMPL_H

#include <gnuradio/analog/agc_cc.h>

namespace gr {
  namespace analog {

    class agc_cc_impl : public agc_cc, kernel::agc_cc
    {
    public:
      agc_cc_impl(float rate = 1e-4, float reference = 1.0,
		  float gain = 1.0);
      ~agc_cc_impl();

      float rate() const { return kernel::agc_cc::rate(); }
      float reference() const { return kernel::agc_cc::reference(); }
      float gain() const { return kernel::agc_cc::gain(); }
      float max_gain() const { return kernel::agc_cc::max_gain(); }

      void set_rate(float rate) { kernel::agc_cc::set_rate(rate); }
      void set_reference(float reference) { kernel::agc_cc::set_reference(reference); }
      void set_gain(float gain) { kernel::agc_cc::set_gain(gain); }
      void set_max_gain(float max_gain) { kernel::agc_cc::set_max_gain(max_gain); }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC_CC_IMPL_H */
