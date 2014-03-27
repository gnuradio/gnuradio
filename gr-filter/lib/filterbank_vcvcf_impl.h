/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_FILTERBANK_VCVCF_IMPL_H
#define	INCLUDED_FILTER_FILTERBANK_VCVCF_IMPL_H

#include <gnuradio/filter/filterbank_vcvcf.h>
#include <gnuradio/filter/filterbank.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace filter {

    class FILTER_API filterbank_vcvcf_impl : public filterbank_vcvcf, kernel::filterbank
    {
    private:
      bool d_updated;
      gr::thread::mutex d_mutex; // mutex to protect set/work access

    public:
      filterbank_vcvcf_impl(const std::vector<std::vector<float> > &taps);
      ~filterbank_vcvcf_impl();

      void set_taps(const std::vector<std::vector<float> > &taps);
      void print_taps();
      std::vector<std::vector<float> > taps() const;

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif
