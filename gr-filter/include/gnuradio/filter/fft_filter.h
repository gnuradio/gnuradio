/* -*- c++ -*- */
/*
 * Copyright 2010,2012,2014 Free Software Foundation, Inc.
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

#include <gnuradio/filter/api.h>
#include <vector>
#include <gnuradio/gr_complex.h>
#include <gnuradio/fft/fft.h>

namespace gr {
  namespace filter {
    namespace kernel {

      /*!
       * \brief Fast FFT filter with float input, float output and float taps
       * \ingroup filter_blk
       *
       * \details
       * This block performs fast convolution using the
       * overlap-and-save algorithm. The filtering is performand in
       * the frequency domain instead of the time domain (see
       * gr::filter::kernel::fir_filter_fff). For an input signal x
       * and filter coefficients (taps) t, we compute y as:
       *
       * \code
       *    y = ifft(fft(x)*fft(t))
       * \endcode
       *
       * This kernel computes the FFT of the taps when they are set to
       * only perform this operation once. The FFT of the input signal
       * x is done every time.
       *
       * Because this is designed as a very low-level kernel
       * operation, it is designed for speed and avoids certain checks
       * in the filter() function itself. The filter function expects
       * that the input signal is a multiple of d_nsamples in the
       * class that's computed internally to be as fast as
       * possible. The function set_taps will return the value of
       * nsamples that can be used externally to check this
       * boundary. Notice that all implementations of the fft_filter
       * GNU Radio blocks (e.g., gr::filter::fft_filter_fff) use this
       * value of nsamples to compute the value to call
       * gr::block::set_output_multiple that ensures the scheduler
       * always passes this block the right number of samples.
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
	std::vector<float>       d_taps;            // stores time domain taps
	gr_complex              *d_xformed_taps;    // Fourier xformed taps

	void compute_sizes(int ntaps);
	int tailsize() const { return d_ntaps - 1; }

      public:
	/*!
	 * \brief Construct an FFT filter for float vectors with the given taps and decimation rate.
	 *
	 * This is the basic implementation for performing FFT filter for fast convolution
	 * in other blocks (e.g., gr::filter::fft_filter_fff).
	 *
	 * \param decimation The decimation rate of the filter (int)
	 * \param taps       The filter taps (vector of float)
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
	 * \brief Returns the taps.
	 */
	std::vector<float> taps() const;

	/*!
	 * \brief Returns the number of taps in the filter.
	 */
        unsigned int ntaps() const;

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
       *
       * \details
       * This block performs fast convolution using the
       * overlap-and-save algorithm. The filtering is performand in
       * the frequency domain instead of the time domain (see
       * gr::filter::kernel::fir_filter_ccc). For an input signal x
       * and filter coefficients (taps) t, we compute y as:
       *
       * \code
       *    y = ifft(fft(x)*fft(t))
       * \endcode
       *
       * This kernel computes the FFT of the taps when they are set to
       * only perform this operation once. The FFT of the input signal
       * x is done every time.
       *
       * Because this is designed as a very low-level kernel
       * operation, it is designed for speed and avoids certain checks
       * in the filter() function itself. The filter function expects
       * that the input signal is a multiple of d_nsamples in the
       * class that's computed internally to be as fast as
       * possible. The function set_taps will return the value of
       * nsamples that can be used externally to check this
       * boundary. Notice that all implementations of the fft_filter
       * GNU Radio blocks (e.g., gr::filter::fft_filter_ccc) use this
       * value of nsamples to compute the value to call
       * gr::block::set_output_multiple that ensures the scheduler
       * always passes this block the right number of samples.
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
	std::vector<gr_complex>  d_taps;            // stores time domain taps
	gr_complex              *d_xformed_taps;    // Fourier xformed taps

	void compute_sizes(int ntaps);
	int tailsize() const { return d_ntaps - 1; }

      public:
	/*!
	 * \brief Construct an FFT filter for complex vectors with the given taps and decimation rate.
	 *
	 * This is the basic implementation for performing FFT filter for fast convolution
	 * in other blocks (e.g., gr::filter::fft_filter_ccc).
	 *
	 * \param decimation The decimation rate of the filter (int)
	 * \param taps       The filter taps (vector of complex)
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
	 * \brief Returns the taps.
	 */
	std::vector<gr_complex> taps() const;

	/*!
	 * \brief Returns the number of taps in the filter.
	 */
        unsigned int ntaps() const;

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



      /*!
       * \brief Fast FFT filter with gr_complex input, gr_complex output and float taps
       * \ingroup filter_blk
       *
       * \details
       * This block performs fast convolution using the
       * overlap-and-save algorithm. The filtering is performand in
       * the frequency domain instead of the time domain (see
       * gr::filter::kernel::fir_filter_ccf). For an input signal x
       * and filter coefficients (taps) t, we compute y as:
       *
       * \code
       *    y = ifft(fft(x)*fft(t))
       * \endcode
       *
       * This kernel computes the FFT of the taps when they are set to
       * only perform this operation once. The FFT of the input signal
       * x is done every time.
       *
       * Because this is designed as a very low-level kernel
       * operation, it is designed for speed and avoids certain checks
       * in the filter() function itself. The filter function expects
       * that the input signal is a multiple of d_nsamples in the
       * class that's computed internally to be as fast as
       * possible. The function set_taps will return the value of
       * nsamples that can be used externally to check this
       * boundary. Notice that all implementations of the fft_filter
       * GNU Radio blocks (e.g., gr::filter::fft_filter_ccf) use this
       * value of nsamples to compute the value to call
       * gr::block::set_output_multiple that ensures the scheduler
       * always passes this block the right number of samples.
       */
      class FILTER_API fft_filter_ccf
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
	std::vector<float>       d_taps;            // stores time domain taps
	gr_complex              *d_xformed_taps;    // Fourier xformed taps

	void compute_sizes(int ntaps);
	int tailsize() const { return d_ntaps - 1; }

      public:
	/*!
	 * \brief Construct an FFT filter for complex vectors with the given taps and decimation rate.
	 *
	 * This is the basic implementation for performing FFT filter for fast convolution
	 * in other blocks (e.g., gr::filter::fft_filter_ccf).
	 *
	 * \param decimation The decimation rate of the filter (int)
	 * \param taps       The filter taps (float)
	 * \param nthreads   The number of threads for the FFT to use (int)
	 */
	fft_filter_ccf(int decimation,
		       const std::vector<float> &taps,
		       int nthreads=1);

	~fft_filter_ccf();

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
	 * \brief Returns the taps.
	 */
	std::vector<float> taps() const;

	/*!
	 * \brief Returns the number of taps in the filter.
	 */
        unsigned int ntaps() const;

	/*!
	 * \brief Returns the actual size of the filter.
         *
         * \details This value could be equal to ntaps, but we ofter
         * build a longer filter to allow us to calculate a more
         * efficient FFT. This value is the actual size of the filters
         * used in the calculation of the overlap-and-save operation.
	 */
        unsigned int filtersize() const;

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
