/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2013 Free Software Foundation, Inc.
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

#include "bin_statistics_f_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace blocks {

    bin_statistics_f::sptr
    bin_statistics_f::make(unsigned int vlen,
                           msg_queue::sptr msgq,
                           feval_dd *tune,
                           size_t tune_delay,
                           size_t dwell_delay)
    {
      return gnuradio::get_initial_sptr
        (new bin_statistics_f_impl(vlen, msgq, tune,
                                   tune_delay, dwell_delay));
    }

    bin_statistics_f_impl::bin_statistics_f_impl(unsigned int vlen,
                                                 msg_queue::sptr msgq,
                                                 feval_dd *tune,
                                                 size_t tune_delay,
                                                 size_t dwell_delay)
      : sync_block("bin_statistics_f",
                      io_signature::make(1, 1, sizeof(float) * vlen),
                      io_signature::make(0, 0, 0)),
        d_vlen(vlen), d_msgq(msgq), d_tune(tune),
        d_tune_delay(tune_delay), d_dwell_delay(dwell_delay),
        d_center_freq(0), d_delay(0),
        d_max(vlen)
    {
      enter_init();
    }

    bin_statistics_f_impl::~bin_statistics_f_impl()
    {
    }

    void
    bin_statistics_f_impl::enter_init()
    {
      d_state = ST_INIT;
      d_delay = 0;
    }

    void
    bin_statistics_f_impl::enter_tune_delay()
    {
      d_state = ST_TUNE_DELAY;
      d_delay = d_tune_delay;
      d_center_freq = d_tune->calleval(0);
    }

    void
    bin_statistics_f_impl::enter_dwell_delay()
    {
      d_state = ST_DWELL_DELAY;
      d_delay = d_dwell_delay;
      reset_stats();
    }

    void
    bin_statistics_f_impl::leave_dwell_delay()
    {
      send_stats();
    }

    int
    bin_statistics_f_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const float *input = (const float*)input_items[0];
      size_t vlen = d_max.size();

      int n = 0;
      int t;

      while(n < noutput_items) {
        switch(d_state) {

          case ST_INIT:
            enter_tune_delay();
            break;

          case ST_TUNE_DELAY:
            t = std::min(noutput_items - n, int(d_delay));
            n += t;
            d_delay -= t;
            assert(d_delay >= 0);
            if(d_delay == 0)
              enter_dwell_delay();
            break;

          case ST_DWELL_DELAY:
            t = std::min(noutput_items - n, int(d_delay));
            for(int i = 0; i < t; i++) {
              accrue_stats(&input[n * vlen]);
              n++;
            }
            d_delay -= t;
            assert(d_delay >= 0);
            if(d_delay == 0) {
              leave_dwell_delay();
              enter_tune_delay();
            }
            break;

          default:
            assert(0);
          }
      }

      return noutput_items;
    }

    //////////////////////////////////////////////////////////////////////////
    //           virtual methods for gathering stats
    //////////////////////////////////////////////////////////////////////////

    void
    bin_statistics_f_impl::reset_stats()
    {
      for (size_t i = 0; i < vlen(); i++){
        d_max[i] = 0;
      }
    }

    void
    bin_statistics_f_impl::accrue_stats(const float *input)
    {
      for(size_t i = 0; i < vlen(); i++) {
        d_max[i] = std::max(d_max[i], input[i]);    // compute per bin maxima
      }
    }

    void
    bin_statistics_f_impl::send_stats()
    {
      if(msgq()->full_p())    // if the queue is full, don't block, drop the data...
        return;

      // build & send a message
      message::sptr msg = message::make(0, center_freq(), vlen(), vlen() * sizeof(float));
      memcpy(msg->msg(), &d_max[0], vlen() * sizeof(float));
      msgq()->insert_tail(msg);
    }

  } /* namespace blocks */
} /* namespace gr */

