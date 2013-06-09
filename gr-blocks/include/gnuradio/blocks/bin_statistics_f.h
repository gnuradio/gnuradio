/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_BIN_STATISTICS_F_H
#define INCLUDED_GR_BIN_STATISTICS_F_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>
#include <gnuradio/feval.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief control scanning and record frequency domain statistics
     * \ingroup misc_blk
     */
    class BLOCKS_API bin_statistics_f : virtual public sync_block
    {
    protected:
      std::vector<float> d_max; // per bin maxima

      virtual size_t vlen() const = 0;
      virtual double center_freq() const = 0;
      virtual gr::msg_queue::sptr msgq() const = 0;

      virtual void reset_stats() = 0;
      virtual void accrue_stats(const float *input) = 0;
      virtual void send_stats() = 0;

    public:
      // gr::blocks::bin_statistics_f::sptr
      typedef boost::shared_ptr<bin_statistics_f> sptr;

      /*!
       * Build a bin statistics block. See qa_bin_statistics.py and
       * gr-uhd/examples/python/usrp_spectrum_sense.py for examples of
       * its use, specifically how to use the callback function.
       *
       * \param vlen vector length
       * \param msgq message queue
       * \param tune a feval_dd callback function
       * \param tune_delay number of samples for the tune delay
       * \param dwell_delay number of samples for the dwell delay
       */
      static sptr make(unsigned int vlen,      // vector length
                       gr::msg_queue::sptr msgq,
                       feval_dd *tune,      // callback
                       size_t tune_delay,      // samples
                       size_t dwell_delay);    // samples
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BIN_STATISTICS_F_H */
