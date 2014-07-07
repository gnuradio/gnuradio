/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H
#define	INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H

#include <gnuradio/filter/pfb_interpolator_ccf.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace filter {

    class FILTER_API pfb_interpolator_ccf_impl : public pfb_interpolator_ccf, kernel::polyphase_filterbank
    {
    private:
      bool         d_updated;
      unsigned int d_rate;
      gr::thread::mutex d_mutex; // mutex to protect set/work access

    public:
      pfb_interpolator_ccf_impl(unsigned int interp,
				const std::vector<float> &taps);

      ~pfb_interpolator_ccf_impl();

      void set_taps(const std::vector<float> &taps);
      void print_taps();
      std::vector<std::vector<float> > taps() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H */
