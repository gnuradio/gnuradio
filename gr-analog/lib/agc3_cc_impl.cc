/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012,2013 Free Software Foundation, Inc.
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

#include <float.h>
#include <vector>

#include "agc3_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
  namespace analog {

    agc3_cc::sptr
    agc3_cc::make(float attack_rate, float decay_rate,
                  float reference, float gain, int iir_update_decim)
    {
      return gnuradio::get_initial_sptr
        (new agc3_cc_impl(attack_rate, decay_rate,
                          reference, gain, iir_update_decim));
    }

    agc3_cc_impl::agc3_cc_impl(float attack_rate, float decay_rate,
                               float reference, float gain, int iir_update_decim)
    : sync_block("agc3_cc",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_attack(attack_rate), d_decay(decay_rate),
      d_reference(reference), d_gain(gain), d_max_gain(65536),
      d_reset(true), d_iir_update_decim(iir_update_decim)
    {
      set_output_multiple(iir_update_decim*4);
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

#ifdef __GNUC__
    // Compute a linear average on reset (no expected)
    if(__builtin_expect(d_reset, false)) {
      float mags[noutput_items]  __attribute__ ((aligned (16)));
      volk_32fc_magnitude_32f(mags, &in[0], noutput_items);
#else
    // Compute a linear average on reset (no expected)
	if(!d_reset) {
      _declspec(align(16)) std::vector<float> mags(noutput_items) ;
      volk_32fc_magnitude_32f(&mags[0], &in[0], noutput_items);
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
#ifdef _MSC_VER
        __declspec(align(16)) std::vector<float> mag_sq(noutput_items/d_iir_update_decim) ;
        __declspec(align(16)) std::vector<float> inv_mag(noutput_items/d_iir_update_decim) ;
#else
        float mag_sq[noutput_items/d_iir_update_decim] __attribute__ ((aligned (16)));
        float inv_mag[noutput_items/d_iir_update_decim] __attribute__ ((aligned (16)));
#endif

        // generate squared magnitudes at decimated rate (gather operation)
        for(int i=0; i<noutput_items/d_iir_update_decim; i++){
            int idx = i*d_iir_update_decim;
            mag_sq[i] = in[idx].real()*in[idx].real() + in[idx].imag()*in[idx].imag();
        }

        // compute inverse square roots
        volk_32f_invsqrt_32f(&inv_mag[0], &mag_sq[0], noutput_items/d_iir_update_decim);

        // apply updates
        for(int i=0; i<noutput_items/d_iir_update_decim; i++){
            float magi = inv_mag[i];
#if defined(_MSC_VER) && _MSC_VER < 1900
            if(!_finite(magi)){
#else
            if(std::isfinite(magi)){
#endif
                float rate = (magi > d_gain/d_reference)?d_decay:d_attack;
                d_gain = d_gain*(1-rate) + d_reference*magi*rate;
            } else {
                d_gain = d_gain*(1-d_decay);
            }
            for(int j=i*d_iir_update_decim; j<(i+1)*d_iir_update_decim; j++){
                out[j] = in[j] * d_gain;
            }
        }
      }
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
