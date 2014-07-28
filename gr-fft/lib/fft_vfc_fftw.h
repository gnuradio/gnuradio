/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H
#define INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H

#include <gnuradio/fft/fft_vfc.h>
#include <gnuradio/fft/fft.h>

namespace gr {
  namespace fft {

    class FFT_API fft_vfc_fftw : public fft_vfc
    {
    private:
      fft_complex          *d_fft;
      unsigned int          d_fft_size;
      std::vector<float>    d_window;
      bool                  d_forward;

    public:
      fft_vfc_fftw(int fft_size, bool forward,
		   const std::vector<float> &window,
		   int nthreads=1);

      ~fft_vfc_fftw();

      void set_nthreads(int n);
      int nthreads() const;
      bool set_window(const std::vector<float> &window);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H */
