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


#ifndef INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H
#define	INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H

#include <gnuradio/filter/pfb_synthesizer_ccf.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace filter {

    // While this is a polyphase_filterbank, we don't use the normal
    // parent class because we have to use the fir_filter_with_buffer
    // objects instead of normal filters.

    class FILTER_API pfb_synthesizer_ccf_impl : public pfb_synthesizer_ccf
    {
    private:
      bool	        d_updated;
      unsigned int      d_numchans;
      unsigned int      d_taps_per_filter;
      fft::fft_complex *d_fft;
      std::vector< kernel::fir_filter_with_buffer_ccf*> d_filters;
      std::vector< std::vector<float> >                 d_taps;
      int              d_state;
      std::vector<int> d_channel_map;
      unsigned int     d_twox;
      gr::thread::mutex     d_mutex; // mutex to protect set/work access

      /*!
       * \brief Tap setting algorithm for critically sampled channels
       */
      void set_taps1(const std::vector<float> &taps);

      /*!
       * \brief Tap setting algorithm for 2x over-sampled channels
       */
      void set_taps2(const std::vector<float> &taps);


    public:
      pfb_synthesizer_ccf_impl(unsigned int numchans,
			       const std::vector<float> &taps,
			       bool twox);
      ~pfb_synthesizer_ccf_impl();

      void set_taps(const std::vector<float> &taps);
      std::vector<std::vector<float> > taps() const;
      void print_taps();

      void set_channel_map(const std::vector<int> &map);
      std::vector<int> channel_map() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H */
