/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_IMPL_H
#define INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_IMPL_H

#include <gnuradio/digital/correlate_and_sync_cc.h>
#include <gnuradio/filter/fft_filter.h>

using namespace gr::filter;

namespace gr {
  namespace digital {

    class correlate_and_sync_cc_impl : public correlate_and_sync_cc
    {
    private:
      std::vector<gr_complex> d_symbols;
      unsigned int d_sps;
      float d_center_first_symbol;
      float d_thresh;
      kernel::fft_filter_ccc  *d_filter;

      int d_last_index;

    public:
      correlate_and_sync_cc_impl(const std::vector<gr_complex> &symbols,
                                 const std::vector<float> &filter,
                                 unsigned int sps, unsigned int nfilts=32);
      ~correlate_and_sync_cc_impl();

      std::vector<gr_complex> symbols() const;
      void set_symbols(const std::vector<gr_complex> &symbols);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_IMPL_H */
