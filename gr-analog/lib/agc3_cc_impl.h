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

#ifndef INCLUDED_ANALOG_AGC3_IMPL_CC_H
#define INCLUDED_ANALOG_AGC3_IMPL_CC_H

#include <gnuradio/analog/agc3_cc.h>

namespace gr {
  namespace analog {

    class agc3_cc_impl : public agc3_cc
    {
    public:
      agc3_cc_impl(float attack_rate = 1e-1, float decay_rate = 1e-2,
                   float reference = 1.0, float gain = 1.0, int iir_update_decim=1);
      ~agc3_cc_impl();

      float attack_rate() const { return d_attack; }
      float decay_rate() const  { return d_decay; }
      float reference() const { return d_reference; }
      float gain() const { return d_gain; }
      float max_gain() const { return d_max_gain; }

      void set_attack_rate(float rate) { d_attack = rate; }
      void set_decay_rate(float rate) { d_decay = rate; }
      void set_reference(float reference) { d_reference = reference; }
      void set_gain(float gain) { d_gain = gain; }
      void set_max_gain(float max_gain) { d_max_gain = max_gain; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

    private:
      float d_attack;
      float d_decay;
      float d_reference;
      float d_gain;
      float d_max_gain;
      bool d_reset;
      int d_iir_update_decim;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC3_CC_IMPL_H */
