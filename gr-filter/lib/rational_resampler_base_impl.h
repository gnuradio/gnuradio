/* -*- c++ -*- */
/*
 * Copyright 2005,2012,2018 Free Software Foundation, Inc.
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

#ifndef RATIONAL_RESAMPLER_IMPL_BASE_H
#define	RATIONAL_RESAMPLER_IMPL_BASE_H

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/rational_resampler_base.h>

namespace gr {
  namespace filter {

    template <class IN_T, class OUT_T, class TAP_T>
    class FILTER_API rational_resampler_base_impl : public rational_resampler_base<IN_T,OUT_T,TAP_T>
    {
    private:
      unsigned d_history;
      unsigned d_interpolation;
      unsigned d_decimation;
      unsigned d_ctr;
      std::vector<TAP_T> d_new_taps;
      std::vector<kernel::fir_filter<IN_T,OUT_T,TAP_T> *> d_firs;
      bool d_updated;

      void install_taps(const std::vector<TAP_T> &taps);

    public:
      rational_resampler_base_impl(unsigned interpolation, unsigned decimation,
		  const std::vector<TAP_T> &taps);

      ~rational_resampler_base_impl();

      unsigned history() const { return d_history; }
      void set_history(unsigned history) { d_history = history; }

      unsigned interpolation() const { return d_interpolation; }
      unsigned decimation() const { return d_decimation; }

      void set_taps(const std::vector<TAP_T> &taps);
      std::vector<TAP_T> taps() const;

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      int  general_work(int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* RATIONAL_RESAMPLER_IMPL_BASE_H */
