/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_CPFSK_BC_IMPL_H
#define INCLUDED_ANALOG_CPFSK_BC_IMPL_H

#include <gnuradio/analog/cpfsk_bc.h>

namespace gr {
  namespace analog {

    class cpfsk_bc_impl : public cpfsk_bc
    {
    private:
      int   d_samples_per_sym;	// Samples per symbol, square pulse
      float d_freq;		// Modulation index*pi/samples_per_sym
      float d_ampl;		// Output amplitude
      float d_phase;		// Current phase

    public:
      cpfsk_bc_impl(float k, float ampl, int samples_per_sym);
      ~cpfsk_bc_impl();

      void set_amplitude(float amplitude) { d_ampl = amplitude; }
      float amplitude() { return d_ampl; }
      float freq() { return d_freq; }
      float phase() { return d_phase; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_CPFSK_BC_IMPL_H */
