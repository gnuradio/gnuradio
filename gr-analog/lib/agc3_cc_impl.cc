/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>

#include "agc3_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace analog {

    agc3_cc::sptr
    agc3_cc::make(float attack_rate, float decay_rate,
                  float reference, float gain)
    {
      return gnuradio::get_initial_sptr
        (new agc3_cc_impl(attack_rate, decay_rate,
                          reference, gain));
    }

    agc3_cc_impl::agc3_cc_impl(float attack_rate, float decay_rate,
                               float reference, float gain)
    : sync_block("agc3_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_attack(attack_rate), d_decay(decay_rate),
      d_reference(reference), d_gain(gain), d_max_gain(65536),
      d_reset(true)
    {
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
    }

    agc3_cc_impl::~agc3_cc_impl()
    {
    }

    int
    agc3_cc_impl::work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      // Compute magnitude of each sample
#ifdef __GNUC__
      float mags[noutput_items]  __attribute__ ((aligned (16)));
      volk_32fc_magnitude_32f(mags, &in[0], noutput_items);
      // Compute a linear average on reset (no expected)  
      if(__builtin_expect(d_reset, false)) {
#else   
	std::vector<float> mags(noutput_items);
	volk_32fc_magnitude_32f(&mags[0], &in[0], noutput_items);
	// Compute a linear average on reset (no expected)  
	if(!d_reset) {
#endif
        float mag(0.0);
        for(int i=0; i<noutput_items; i++) {
            mag += mags[i];
        }
        d_gain = d_reference * (noutput_items/mag);

        if(d_gain < 0.0)
          d_gain = 10e-5;

        if(d_max_gain > 0.0 && d_gain > d_max_gain) {
          d_gain = d_max_gain;
        }

        // scale output values
        for(int i=0; i<noutput_items; i++){
            out[i] = in[i] * d_gain;
        }
        d_reset = false;
      }
      else {
        // Otherwise perform a normal iir update
        for(int i=0; i<noutput_items; i++) {
          float newlevel = mags[i]; // abs(in[i]);
          float rate = (newlevel > d_reference/d_gain)?d_attack:d_decay;
          d_gain = newlevel==0?(d_gain*(1-rate)):(d_gain*(1-rate)) + (d_reference/newlevel)*rate;
          out[i] = in[i] * d_gain;
        }
      } 
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
