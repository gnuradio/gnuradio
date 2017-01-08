/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
 * Copyright (C) 2016-2017  Andy Walls <awalls.cx18@gmail.com>
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

#ifndef INCLUDED_DIGITAL_SYMBOL_SYNC_MM_CC_H
#define INCLUDED_DIGITAL_SYMBOL_SYNC_MM_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Mueller and Müller (M&M) based symbol synchronizer block with complex input, complex output.
     * \ingroup synchronizers_blk
     *
     * \details
     * This implements the modified Mueller and Müller (M&M) discrete-time
     * error-tracking synchronizer.
     *
     * For this block to work properly, the input stream must meet the
     * following requirements:
     *
     * 1. the input pulses must have peaks (not flat), which usually can
     * be implemented by using a matched filter before this block.
     *
     * 2. the input pulse peaks should nominally have a magnitude of 1.0.
     *
     * 3. the input should be properly rotated in I & Q so that the
     * pulse peaks line up with the 45 degree QPSK constellation points:
     * 1.0/sqrt(2.0) * [ 1+1j, -1+1j, -1-1j, 1-1j ].
     *
     * 4. the input baseband can be either QPSK or BPSK rotated to
     * line up with the 45 degree QPSK constellation points.
     *
     * The complex version here is based on: Modified Mueller and
     * Muller clock recovery circuit:
     *
     *    G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller
     *    and Muller algorithm," Electronics Letters, Vol. 31, no. 13, 22
     *    June 1995, pp. 1032 - 1033.
     */

    class DIGITAL_API symbol_sync_mm_cc : virtual public block
    {
    public:
      // gr::digital::symbol_sync_mm_cc::sptr
      typedef boost::shared_ptr<symbol_sync_mm_cc> sptr;

      /*!
       * Make a M&M symbol synchronizer block.
       *
       * \param sps
       * User specified nominal clock period in samples per symbol.
       *
       * \param loop_bw
       * Approximate normailzed loop bandwidth of the symbol clock tracking
       * loop. It should be in the range (0.0f, 0.5f).
       *
       * \param damping_factor
       * Damping factor of the symbol clock tracking loop.
       * Damping < 1.0f is an under-damped loop.
       * Damping = 1.0f is a critically-damped loop.
       * Damping > 1.0f is an over-damped loop.
       * One should generally use an over-damped loop for symbol clock tracking.
       *
       * \param max_deviation
       * Maximum absolute deviation of the average clock period estimate
       * from the user specified nominal clock period in samples per symbol.
       *
       * \param osps
       * The number of output samples per symbol (default=1).
       */
      static sptr make(float sps,
                       float loop_bw,
                       float damping_factor = 2.0f,
		       float max_deviation = 1.5f,
                       int osps = 1);
      
      virtual float loop_bandwidth() const = 0;
      virtual float damping_factor() const = 0;
      virtual float alpha() const = 0;
      virtual float beta() const = 0;

      virtual void set_loop_bandwidth (float fn_norm) = 0;
      virtual void set_damping_factor (float zeta) = 0;
      virtual void set_alpha (float alpha) = 0;
      virtual void set_beta (float beta) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_SYMBOL_SYNC_MM_CC_H */
