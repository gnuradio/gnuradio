/* -*- c++ -*- */
/*
 * Copyright 2005-2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_THROTTLE_IMPL_H
#define INCLUDED_GR_THROTTLE_IMPL_H

#include <gnuradio/blocks/throttle.h>

namespace gr {
  namespace blocks {

    class throttle_impl : public throttle
    {
    private:
      boost::system_time d_start;
      size_t d_itemsize;
      uint64_t d_total_samples;
      double d_samps_per_tick, d_samps_per_us;
      bool d_ignore_tags;

    public:
      throttle_impl(size_t itemsize, double samples_per_sec, bool ignore_tags=true);
      ~throttle_impl();

      // Overloading gr::block::start to reset timer
      bool start();

      void setup_rpc();

      void set_sample_rate(double rate);
      double sample_rate() const;

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THROTTLE_IMPL_H */
