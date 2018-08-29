/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
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

#ifndef INTERP_FIR_FILTER_IMPL_H
#define	INTERP_FIR_FILTER_IMPL_H

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/interp_fir_filter.h>
#include <vector>

namespace gr {
  namespace filter {

    template <class IN_T, class OUT_T, class TAP_T>
    class FILTER_API interp_fir_filter_impl : public interp_fir_filter<IN_T,OUT_T,TAP_T>
    {
    private:
      bool d_updated;
      std::vector< kernel::fir_filter<IN_T,OUT_T,TAP_T> *> d_firs;
      std::vector<TAP_T> d_new_taps;

      void install_taps(const std::vector<TAP_T> &taps);

    public:
      interp_fir_filter_impl(unsigned interpolation,
		  const std::vector<TAP_T> &taps);

      ~interp_fir_filter_impl();

      void set_taps(const std::vector<TAP_T> &taps);
      std::vector<TAP_T> taps() const;

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INTERP_FIR_FILTER_IMPL_H */
