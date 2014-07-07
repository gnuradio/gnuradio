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

#ifndef INCLUDED_ANALOG_DPLL_BB_IMPL_H
#define INCLUDED_ANALOG_DPLL_BB_IMPL_H

#include <gnuradio/analog/dpll_bb.h>

namespace gr {
  namespace analog {

    class dpll_bb_impl : public dpll_bb
    {
    private:
      unsigned char d_restart;
      float d_pulse_phase, d_pulse_frequency;
      float d_gain, d_decision_threshold;

    public:
      dpll_bb_impl(float period, float gain);
      ~dpll_bb_impl();

      void set_gain(float gain) { d_gain = gain; }
      void set_decision_threshold(float thresh) { d_decision_threshold = thresh; }

      float gain() const { return d_gain; }
      float freq() const { return d_pulse_frequency; }
      float phase() const { return d_pulse_phase; }
      float decision_threshold() const { return d_decision_threshold; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_DPLL_BB_IMPL_H */
