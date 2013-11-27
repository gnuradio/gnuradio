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

#ifndef INCLUDED_GR_THROTTLE_H
#define INCLUDED_GR_THROTTLE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief throttle flow of samples such that the average rate does
     * not exceed samples_per_sec.
     * \ingroup misc_blk
     *
     * \details
     * input: one stream of itemsize; output: one stream of itemsize
     *
     * N.B. this should only be used in GUI apps where there is no
     * other rate limiting block. It is not intended nor effective at
     * precisely controlling the rate of samples. That should be
     * controlled by a source or sink tied to sample clock. E.g., a
     * USRP or audio card.
     */
    class BLOCKS_API throttle : virtual public sync_block
    {
    public:
      typedef boost::shared_ptr<throttle> sptr;

      static sptr make(size_t itemsize, double samples_per_sec, bool ignore_tags=true);

      //! Sets the sample rate in samples per second.
      virtual void set_sample_rate(double rate) = 0;

      //! Get the sample rate in samples per second.
      virtual double sample_rate() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THROTTLE_H */
