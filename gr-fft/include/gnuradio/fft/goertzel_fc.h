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

#ifndef INCLUDED_FFT_GOERTZEL_FC_H
#define INCLUDED_FFT_GOERTZEL_FC_H

#include <gnuradio/fft/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace fft {

    /*!
     * \brief Goertzel single-bin DFT calculation.
     * \ingroup fourier_analysis_blk
     */
    class FFT_API goertzel_fc : virtual public sync_decimator
    {
    public:

      // gr::fft::goertzel_fc::sptr
      typedef boost::shared_ptr<goertzel_fc> sptr;

      static sptr make(int rate, int len, float freq);

      virtual void set_freq (float freq) = 0;

      virtual void set_rate (int rate) = 0;

      virtual float freq() = 0;

      virtual int rate() = 0;
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_GOERTZEL_FC_H */
