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


#ifndef INCLUDED_PFB_ARB_RESAMPLER_CCC_IMPL_H
#define	INCLUDED_PFB_ARB_RESAMPLER_CCC_IMPL_H

#include <gnuradio/filter/pfb_arb_resampler_ccc.h>
#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace filter {

    class FILTER_API pfb_arb_resampler_ccc_impl : public pfb_arb_resampler_ccc
    {
    private:
      kernel::pfb_arb_resampler_ccc *d_resamp;
      bool d_updated;
      gr::thread::mutex d_mutex; // mutex to protect set/work access

    public:
      pfb_arb_resampler_ccc_impl(float rate,
                                 const std::vector<gr_complex> &taps,
                                 unsigned int filter_size);

      ~pfb_arb_resampler_ccc_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      void set_taps(const std::vector<gr_complex> &taps);
      std::vector<std::vector<gr_complex> > taps() const;
      void print_taps();

      void set_rate(float rate);
      void set_phase(float ph);
      float phase() const;

      unsigned int interpolation_rate() const;
      unsigned int decimation_rate() const;
      float fractional_rate() const;
      unsigned int taps_per_filter() const;

      int group_delay() const;
      float phase_offset(float freq, float fs);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_CCC_IMPL_H */
