/* -*- c++ -*- */
/*
 * Copyright 2002,2011,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <cmath>
#include <gnuradio/fft/goertzel.h>

namespace gr {
  namespace fft {

    goertzel::goertzel(int rate, int len, float freq)
    {
      set_params(rate, len, freq);
    }

    void
    goertzel::set_params(int rate, int len, float freq)
    {
      d_d1 = 0.0;
      d_d2 = 0.0;

      float w = 2.0*M_PI*freq/rate;
      d_wr = 2.0*std::cos(w);
      d_wi = std::sin(w);
      d_len = len;
      d_processed = 0;
    }

    gr_complex
    goertzel::batch(float *in)
    {
      d_d1 = 0.0;
      d_d2 = 0.0;

      for(int i = 0; i < d_len; i++)
	input(in[i]);

      return output();
    }

    void
    goertzel::input(const float &input)
    {
      float y = input + d_wr*d_d1 - d_d2;
      d_d2 = d_d1;
      d_d1 = y;
      d_processed++;
    }

    gr_complex
    goertzel::output()
    {
      gr_complex out((0.5*d_wr*d_d1-d_d2)/d_len, (d_wi*d_d1)/d_len);
      d_d1 = 0.0;
      d_d2 = 0.0;
      d_processed = 0;
      return out;
    }

  } /* namespace fft */
}/* namespace gr */

