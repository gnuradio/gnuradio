/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PROBE_RATE_IMPL_H
#define INCLUDED_GR_PROBE_RATE_IMPL_H

#include <gnuradio/blocks/probe_rate.h>

namespace gr {
  namespace blocks {

    class probe_rate_impl : public probe_rate
    {
    private:
      double d_alpha, d_beta, d_avg;
      double d_min_update_time;
      boost::posix_time::ptime d_last_update;
      uint64_t d_lastthru;
      size_t d_itemsize;
      void setup_rpc();

      const pmt::pmt_t d_port;
      const pmt::pmt_t d_dict_avg, d_dict_now;

    public:
      probe_rate_impl(size_t itemsize, double update_rate_ms, double alpha = 0.0001);
      ~probe_rate_impl();
      void set_alpha(double alpha);
      double rate();
      double timesincelast();
      bool start();
      bool stop();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

    }; // end class

  } /* namespace blocks */
} /* namespace gr */

#endif
