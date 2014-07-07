/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FILTER_DC_BLOCKER_FF_H
#define	INCLUDED_FILTER_DC_BLOCKER_FF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief a computationally efficient controllable DC blocker
     * \ingroup filter_blk
     *
     * \details
     * This block implements a computationally efficient DC blocker
     * that produces a tighter notch filter around DC for a smaller
     * group delay than an equivalent FIR filter or using a single
     * pole IIR filter (though the IIR filter is computationally
     * cheaper).
     *
     * The block defaults to using a delay line of length 32 and the
     * long form of the filter. Optionally, the delay line length can
     * be changed to alter the width of the DC notch (longer lines
     * will decrease the width).
     *
     * The long form of the filter produces a nearly flat response
     * outside of the notch but at the cost of a group delay of 2D-2.
     *
     * The short form of the filter does not have as flat a response
     * in the passband but has a group delay of only D-1 and is
     * cheaper to compute.
     *
     * The theory behind this block can be found in the paper:
     *
     *    <B><EM>R. Yates, "DC Blocker Algorithms," IEEE Signal Processing Magazine,
     *        Mar. 2008, pp 132-134.</EM></B>
     */
    class FILTER_API dc_blocker_ff : virtual public sync_block
    {
    public:

      // gr::filter::dc_blocker_ff::sptr
      typedef boost::shared_ptr<dc_blocker_ff> sptr;

      /*!
       * Make a DC blocker block.
       *
       * \param D          (int) the length of the delay line
       * \param long_form  (bool) whether to use long (true, default) or short form
       */
      static sptr make(int D, bool long_form=true);

      virtual int group_delay() = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_DC_BLOCKER_FF_H */
