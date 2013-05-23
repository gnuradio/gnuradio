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

#include "agc3_cc_impl.h"
#include <gr_io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace analog {

    agc3_cc::sptr
    agc3_cc::make(float attack_rate, float decay_rate, float reference)
    {
      return gnuradio::get_initial_sptr (new agc3_cc_impl(attack_rate, decay_rate, reference));
    }

    agc3_cc_impl::agc3_cc_impl(float attack_rate, float decay_rate, float reference)
      : gr_sync_block("agc3_cc",
		      gr_make_io_signature(1, 1, sizeof(gr_complex)),
		      gr_make_io_signature(1, 1, sizeof(gr_complex))),
        d_attack(attack_rate), d_decay(decay_rate),
        d_reference(reference), d_gain(1.0),
        d_reset(true)
    {
    set_alignment(volk_get_alignment());
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
#else   
        float mags[noutput_items];
#endif
      volk_32fc_magnitude_32f(mags, &in[0], noutput_items);
      // Compute a linear average on reset (no expected)  
      if(__builtin_expect (d_reset, false)){
        float mag;
        for(int i=0; i<noutput_items; i++){
            mag += mags[i];
        }
        d_gain = d_reference * (noutput_items/mag);
        // scale output values
        for(int i=0; i<noutput_items; i++){
            out[i] = in[i] * d_gain;
        }
        d_reset = false;
        printf("computed initial mean on %d values = %f\n", noutput_items, d_gain);
      } else {
        // Otherwise perform a normal iir update
        for(int i=0; i<noutput_items; i++){
          float newlevel = mags[i]; // abs(in[i]);
          float rate = (newlevel > d_reference/d_gain)?d_attack:d_decay;
          d_gain = (d_gain*(1-rate)) + (d_reference/newlevel)*rate;
          out[i] = in[i] * d_gain;
        }
      } 
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
