/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FFT_IMPL_GOERTZEL_H
#define INCLUDED_FFT_IMPL_GOERTZEL_H

#include <gnuradio/fft/api.h>
#include <gnuradio/types.h>

namespace gr {
  namespace fft {

    /*!
     * \brief Implements Goertzel single-bin DFT calculation
     * \ingroup misc
     */
    class FFT_API goertzel
    {
    public:
      goertzel(int rate, int len, float freq);

      void set_params(int rate, int len, float freq);

      // Process a input array
      gr_complex batch(float *in);

      // Process sample by sample
      void input(const float &in);
      gr_complex output();
      bool ready() const { return d_processed == d_len; }

    private:
      float d_d1;
      float d_d2;
      float d_wr;
      float d_wi;
      int   d_len;
      int   d_processed;
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_IMPL_GOERTZEL_H */
