/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_FILTER_DELAY_FC_H
#define INCLUDED_FILTER_FILTER_DELAY_FC_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Filter-Delay Combination Block.
     * \ingroup filter_blk
     *
     * \details
     * The block takes one or two float stream and outputs a complex
     * stream.
     *
     * If only one float stream is input, the real output is a delayed
     * version of this input and the imaginary output is the filtered
     * output.
     *
     * If two floats are connected to the input, then the real output
     * is the delayed version of the first input, and the imaginary
     * output is the filtered output.
     *
     * The delay in the real path accounts for the group delay
     * introduced by the filter in the imaginary path. The filter taps
     * needs to be calculated before initializing this block.
     */
    class FILTER_API filter_delay_fc : virtual public sync_block
    {
    public:

      // gr::filter::filter_delay_fc::sptr
      typedef boost::shared_ptr<filter_delay_fc> sptr;

      /*!
       * Build a filter with delay block.
       */
      static sptr make(const std::vector<float> &taps);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FILTER_DELAY_FC_H */
