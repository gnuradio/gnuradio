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

#include <fft/api.h>
#include <gr_sync_decimator.h>

namespace gr {
  namespace fft {

    class FFT_API goertzel_fc : virtual public gr_sync_decimator
    {
    public:

      // gr::fft::goertzel_fc::sptr
      typedef boost::shared_ptr<goertzel_fc> sptr;
      
      /*!
       * \brief Goertzel single-bin DFT calculation.
       * \ingroup dft_blk
       */
      static FFT_API sptr make(int rate, int len, float freq);

      virtual void set_freq (float freq)
      { throw std::runtime_error("goertzel_fc::set_freq not implemented.\n"); }
	
      virtual void set_rate (int rate)
      { throw std::runtime_error("goertzel_fc::set_rate not implemented.\n"); }

      float freq()
      { throw std::runtime_error("goertzel_fc::freq not implemented.\n"); }
	
      int rate()
      { throw std::runtime_error("goertzel_fc::rate not implemented.\n"); }
	
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_GOERTZEL_FC_H */

