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

#ifndef INCLUDED_BLOCKS_RMS_FF_H
#define INCLUDED_BLOCKS_RMS_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief RMS average power
     * \ingroup math_operators_blk
     */
    class BLOCKS_API rms_ff : virtual public sync_block
    {
    public:
      // gr::blocks::rms_ff::sptr
      typedef boost::shared_ptr<rms_ff> sptr;

      /*!
       * \brief Make an RMS calc. block.
       * \param alpha gain for running average filter.
       */
      static sptr make(double alpha = 0.0001);

      virtual void set_alpha(double alpha) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RMS_FF_H */
