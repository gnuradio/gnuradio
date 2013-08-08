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

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_IMPL_H
#define INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_IMPL_H

#include <gnuradio/digital/ofdm_frame_acquisition.h>

namespace gr {
  namespace digital {
    
    class ofdm_frame_acquisition_impl : public ofdm_frame_acquisition
    {
    private:
      unsigned char slicer(gr_complex x);
      void correlate(const gr_complex *symbol, int zeros_on_left);
      void calculate_equalizer(const gr_complex *symbol, int zeros_on_left);
      gr_complex coarse_freq_comp(int freq_delta, int count);
  
      unsigned int d_occupied_carriers;       // !< \brief number of subcarriers with data
      unsigned int d_fft_length;              // !< \brief length of FFT vector
      unsigned int d_cplen;                   // !< \brief length of cyclic prefix in samples
      unsigned int d_freq_shift_len;          // !< \brief number of surrounding bins to look at for correlation
      std::vector<gr_complex> d_known_symbol; // !< \brief known symbols at start of frame
      std::vector<float> d_known_phase_diff;  // !< \brief factor used in correlation from known symbol
      std::vector<float> d_symbol_phase_diff; // !< \brief factor used in correlation from received symbol
      std::vector<gr_complex> d_hestimate;    // !< channel estimate
      int d_coarse_freq;                      // !< \brief search distance in number of bins
      unsigned int d_phase_count;             // !< \brief accumulator for coarse freq correction
      float d_snr_est;                        // !< an estimation of the signal to noise ratio

      gr_complex *d_phase_lut;  // !< look-up table for coarse frequency compensation

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

    public:
      ofdm_frame_acquisition_impl(unsigned int occupied_carriers, unsigned int fft_length,
				  unsigned int cplen,
				  const std::vector<gr_complex> &known_symbol, 
				  unsigned int max_fft_shift_len=4);
      ~ofdm_frame_acquisition_impl();

      /*!
       * \brief Return an estimate of the SNR of the channel
       */
      float snr() { return d_snr_est; }

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_FRAME_ACQUISITION_IMPL_H */
