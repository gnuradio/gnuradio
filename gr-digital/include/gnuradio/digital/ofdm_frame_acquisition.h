/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_H
#define INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>
#include <vector>

namespace gr {
  namespace digital {
    
    /*!
     * \brief take a vector of complex constellation points in from an
     * FFT and performs a correlation and equalization.
     * \ingroup ofdm_blk
     * \ingroup deprecated_blk
     *
     * \details
     * This block takes the output of an FFT of a received OFDM symbol
     * and finds the start of a frame based on two known symbols. It
     * also looks at the surrounding bins in the FFT output for the
     * correlation in case there is a large frequency shift in the
     * data. This block assumes that the fine frequency shift has
     * already been corrected and that the samples fall in the middle
     * of one FFT bin.
     *
     * It then uses one of those known symbols to estimate the channel
     * response over all subcarriers and does a simple 1-tap
     * equalization on all subcarriers. This corrects for the phase
     * and amplitude distortion caused by the channel.
     */
    class DIGITAL_API ofdm_frame_acquisition : virtual public block
    {
    public:
      // gr::digital::ofdm_frame_acquisition::sptr
      typedef boost::shared_ptr<ofdm_frame_acquisition> sptr;

      /*! 
       * Make an OFDM correlator and equalizer.
       *
       * \param occupied_carriers   The number of subcarriers with data in the received symbol
       * \param fft_length          The size of the FFT vector (occupied_carriers + unused carriers)
       * \param cplen		    The length of the cycle prefix
       * \param known_symbol        A vector of complex numbers representing a known symbol at the
       *                            start of a frame (usually a BPSK PN sequence)
       * \param max_fft_shift_len   Set's the maximum distance you can look between bins for correlation
       */
      static sptr make(unsigned int occupied_carriers, unsigned int fft_length,
		       unsigned int cplen,
		       const std::vector<gr_complex> &known_symbol, 
		       unsigned int max_fft_shift_len=4);
  
      /*!
       * \brief Return an estimate of the SNR of the channel
       */
      virtual float snr() = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_H */
