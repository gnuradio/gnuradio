/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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
#include "config.h"
#endif

#include "fft_filter_ccf_impl.h"
#include <gnuradio/io_signature.h>

#include <math.h>
#include <assert.h>
#include <stdexcept>

namespace gr {
  namespace filter {

    fft_filter_ccf::sptr
    fft_filter_ccf::make(int decimation,
                         const std::vector<float> &taps,
                         int nthreads)
    {
      return gnuradio::get_initial_sptr
        (new fft_filter_ccf_impl
         (decimation, taps, nthreads));
    }

    fft_filter_ccf_impl::fft_filter_ccf_impl(int decimation,
					     const std::vector<float> &taps,
					     int nthreads)
      : sync_decimator("fft_filter_ccf",
                       io_signature::make(1, 1, sizeof(gr_complex)),
                       io_signature::make(1, 1, sizeof(gr_complex)),
                       decimation),
	d_updated(false)
    {
      set_history(1);

      d_filter = new kernel::fft_filter_ccf(decimation, taps, nthreads);

      d_new_taps = taps;
      d_nsamples = d_filter->set_taps(taps);
      set_output_multiple(d_nsamples);
    }

    fft_filter_ccf_impl::~fft_filter_ccf_impl()
    {
      delete d_filter;
    }

    void
    fft_filter_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      d_new_taps = taps;
      d_updated = true;
    }

    std::vector<float>
    fft_filter_ccf_impl::taps() const
    {
      return d_new_taps;
    }

    void
    fft_filter_ccf_impl::set_nthreads(int n)
    {
      if(d_filter)
	d_filter->set_nthreads(n);
    }

    int
    fft_filter_ccf_impl::nthreads() const
    {
      if(d_filter)
	return d_filter->nthreads();
      else
	return 0;
    }

    int
    fft_filter_ccf_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      if (d_updated){
	d_nsamples = d_filter->set_taps(d_new_taps);
	d_updated = false;
	set_output_multiple(d_nsamples);
	return 0;				// output multiple may have changed
      }

      d_filter->filter(noutput_items, in, out);

      return noutput_items;
    }

  } /* namespace filter */
} /* namespace gr */
