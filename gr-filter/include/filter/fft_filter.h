/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_FFT_FILTER_H
#define INCLUDED_FILTER_FFT_FILTER_H

#include <filter/api.h>
#include <vector>
#include <gr_complex.h>
#include <fft/fft.h>

namespace gr {
  namespace filter {
    namespace kernel {
      /*!
       * \brief Fast FFT filter with float input, float output and float taps
       * \ingroup filter_blk
       */
      class FILTER_API fft_filter_fff
      {
      private:
	int			 d_ntaps;
	int			 d_nsamples;
	int			 d_fftsize;         // fftsize = ntaps + nsamples - 1
	int                      d_decimation;
	fft::fft_real_fwd       *d_fwdfft;	    // forward "plan"
	fft::fft_real_rev       *d_invfft;          // inverse "plan"
	int                      d_nthreads;        // number of FFTW threads to use
	std::vector<float>       d_tail;	    // state carried between blocks for overlap-add
	std::vector<float>       d_new_taps;
	gr_complex              *d_xformed_taps;    // Fourier xformed taps
	
	void compute_sizes(int ntaps);
	int tailsize() const { return d_ntaps - 1; }

      public:
	/*!
	 * \brief Construct an FFT filter for float vectors with the given taps and decimation rate.
	 *
	 * This is the basic implementation for performing FFT filter for fast convolution
	 * in other blocks for complex vectors (such as fft_filter_ccc).
	 *
	 * \param decimation The decimation rate of the filter (int)
	 * \param taps       The filter taps (complex)
	 * \param nthreads   The number of threads for the FFT to use (int)
	 */
	fft_filter_fff(int decimation,
		       const std::vector<float> &taps,
		       int nthreads=1);

	~fft_filter_fff();

	/*!
	 * \brief Set new taps for the filter.
	 *
	 * Sets new taps and resets the class properties to handle different sizes
	 * \param taps       The filter taps (complex)
	 */
	int set_taps(const std::vector<float> &taps);
	
	/*!
	 * \brief Set number of threads to use.
	 */
	void set_nthreads(int n);
	
	/*!
	 * \brief Get number of threads being used.
	 */
	int nthreads() const;
	
	/*!
	 * \brief Perform the filter operation
	 *
	 * \param nitems  The number of items to produce
	 * \param input   The input vector to be filtered
	 * \param output  The result of the filter operation
	 */
	int filter(int nitems, const float *input, float *output);
      };

    
      /*!
       * \brief Fast FFT filter with gr_complex input, gr_complex output and gr_complex taps
       * \ingroup filter_blk
       */
      class FILTER_API fft_filter_ccc
      {
      private:
	int			 d_ntaps;
	int			 d_nsamples;
	int			 d_fftsize;         // fftsize = ntaps + nsamples - 1
	int                      d_decimation;
	fft::fft_complex        *d_fwdfft;	    // forward "plan"
	fft::fft_complex        *d_invfft;          // inverse "plan"
	int                      d_nthreads;        // number of FFTW threads to use
	std::vector<gr_complex>  d_tail;	    // state carried between blocks for overlap-add
	std::vector<gr_complex>  d_new_taps;
	gr_complex              *d_xformed_taps;    // Fourier xformed taps
	
	void compute_sizes(int ntaps);
	int tailsize() const { return d_ntaps - 1; }

      public:
	/*!
	 * \brief Construct an FFT filter for complex vectors with the given taps and decimation rate.
	 *
	 * This is the basic implementation for performing FFT filter for fast convolution
	 * in other blocks for complex vectors (such as fft_filter_ccc).
	 *
	 * \param decimation The decimation rate of the filter (int)
	 * \param taps       The filter taps (complex)
	 * \param nthreads   The number of threads for the FFT to use (int)
	 */
	fft_filter_ccc(int decimation,
		       const std::vector<gr_complex> &taps,
		       int nthreads=1);

	~fft_filter_ccc();

	/*!
	 * \brief Set new taps for the filter.
	 *
	 * Sets new taps and resets the class properties to handle different sizes
	 * \param taps       The filter taps (complex)
	 */
	int set_taps(const std::vector<gr_complex> &taps);
	
	/*!
	 * \brief Set number of threads to use.
	 */
	void set_nthreads(int n);
	
	/*!
	 * \brief Get number of threads being used.
	 */
	int nthreads() const;
	
	/*!
	 * \brief Perform the filter operation
	 *
	 * \param nitems  The number of items to produce
	 * \param input   The input vector to be filtered
	 * \param output  The result of the filter operation
	 */
	int filter(int nitems, const gr_complex *input, gr_complex *output);
      };

    } /* namespace kernel */
  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FFT_FILTER_H */
