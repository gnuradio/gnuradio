/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
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

#include "fmdet_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

namespace gr {
  namespace analog {

#define M_TWOPI (2*M_PI)

    fmdet_cf::sptr
    fmdet_cf::make(float samplerate, float freq_low,
		   float freq_high, float scl)
    {
      return gnuradio::get_initial_sptr
	(new fmdet_cf_impl(samplerate, freq_low, freq_high, scl));
    }

    fmdet_cf_impl::fmdet_cf_impl(float samplerate, float freq_low,
				 float freq_high, float scl)
      : sync_block("fmdet_cf",
		      io_signature::make(1, 1, sizeof(gr_complex)),
		      io_signature::make(1, 1, sizeof(float))),
	d_S1(0.1), d_S2(0.1),
	d_S3(0.1), d_S4(0.1)
    {
      //const float h[] = { 0.003118678733, -0.012139843428,  0.027270898036,
      //			 -0.051318579352,  0.090406910552, -0.162926865366,
      //			  0.361885392563,  0.000000000000, -0.361885392563,
      //			  0.162926865366, -0.090406910552,  0.051318579352,
      //			 -0.027270898036,  0.012139843428, -0.003118678733};

      //std::vector<float> taps(15);

      d_freq = 0;
      d_freqhi = freq_high;
      d_freqlo = freq_low;
      set_scale(scl);

      //for(int i = 0; i < 15; i++) {
      //taps[i] = h[i];
      //}
      //  d_filter = gr_fir_util::create_gr_fir_ccf(taps);
    }

    fmdet_cf_impl::~fmdet_cf_impl()
    {
    }

    void
    fmdet_cf_impl::set_scale(float scl)
    {
      float delta = d_freqhi - d_freqlo;
      d_scl = scl;
      d_bias = 0.5*scl*(d_freqhi + d_freqlo) / delta;
    }

    void
    fmdet_cf_impl::set_freq_range(float freq_low, float freq_high)
    {
      d_freqhi = freq_high;
      d_freqlo = freq_low;
      set_scale(d_scl);
    }

    int
    fmdet_cf_impl::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
    {
      const gr_complex *iptr = (gr_complex*)input_items[0];
      float *optr = (float*)output_items[0];
      //const gr_complex *scaleiptr = (gr_complex*)input_items[0];

      int size = noutput_items;

      gr_complex Sdot, S0;
      gr_complex S1=d_S1, S2=d_S2, S3=d_S3, S4=d_S4;
      float d_8 = 8.0;

      while(size-- > 0) {
	S0 = *iptr++;

	Sdot = d_scl * (-S0+d_8*S1-d_8*S1+S4);

	d_freq = (S2.real()*Sdot.imag()-S2.imag()*Sdot.real()) /
	  (S2.real()*S2.real()+S2.imag()*S2.imag());

	S4 = S3;
	S3 = S2;
	S2 = S1;
	S1 = S0;

	*optr++ = d_freq-d_bias;
      }
      d_S1 = S1;
      d_S2 = S2;
      d_S3 = S3;
      d_S4 = S4;
      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
