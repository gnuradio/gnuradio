/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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
#ifndef INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H
#define INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fft_filter_ccc.h>

namespace gr {
  namespace filter {

    class FILTER_API fft_filter_ccc_impl : public fft_filter_ccc
    {
    private:
      int d_nsamples;
      bool d_updated;
      kernel::fft_filter_ccc *d_filter;
      std::vector<gr_complex> d_new_taps;

    public:
      fft_filter_ccc_impl(int decimation,
			  const std::vector<gr_complex> &taps,
			  int nthreads=1);

      ~fft_filter_ccc_impl();

      void set_taps(const std::vector<gr_complex> &taps);
      std::vector<gr_complex> taps() const;

      void set_nthreads(int n);
      int nthreads() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FFT_FILTER_CCC_IMPL_H */
