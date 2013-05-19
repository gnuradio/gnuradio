/* -*- c++ -*- */
/*
 * Copyright 2005-2011 Free Software Foundation, Inc.
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

#include "throttle_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <boost/thread/thread.hpp>

namespace gr {
  namespace blocks {

    throttle::sptr
    throttle::make(size_t itemsize, double samples_per_sec)
    {
      return gnuradio::get_initial_sptr
        (new throttle_impl(itemsize, samples_per_sec));
    }

    throttle_impl::throttle_impl(size_t itemsize,
                                 double samples_per_second)
      : sync_block("throttle",
                      io_signature::make(1, 1, itemsize),
                      io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize)
    {
      set_sample_rate(samples_per_second);
    }

    throttle_impl::~throttle_impl()
    {
    }

    void
    throttle_impl::set_sample_rate(double rate)
    {
      //changing the sample rate performs a reset of state params
      d_start = boost::get_system_time();
      d_total_samples = 0;
      d_samps_per_tick = rate/boost::posix_time::time_duration::ticks_per_second();
      d_samps_per_us = rate/1e6;
    }

    double
    throttle_impl::sample_rate() const
    {
      return d_samps_per_us * 1e6;
    }

    int
    throttle_impl::work(int noutput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
    {
      //calculate the expected number of samples to have passed through
      boost::system_time now = boost::get_system_time();
      boost::int64_t ticks = (now - d_start).ticks();
      uint64_t expected_samps = uint64_t(d_samps_per_tick*ticks);

      //if the expected samples was less, we need to throttle back
      if(d_total_samples > expected_samps) {
        boost::this_thread::sleep(boost::posix_time::microseconds
                                  (long((d_total_samples - expected_samps)/d_samps_per_us)));
      }

      //copy all samples output[i] <= input[i]
      const char *in = (const char *)input_items[0];
      char *out = (char *)output_items[0];
      std::memcpy(out, in, noutput_items * d_itemsize);
      d_total_samples += noutput_items;
      return noutput_items;
    }

    void
    throttle_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<throttle, double>(
            alias(), "sample_rate", &throttle::sample_rate,
            pmt::mp(0.0), pmt::mp(100.0e6), pmt::mp(0.0),
            "Hz", "Sample Rate", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_set<throttle, double>(
            alias(), "sample_rate", &throttle::set_sample_rate,
            pmt::mp(0.0), pmt::mp(100.0e6), pmt::mp(0.0),
            "Hz", "Sample Rate", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
