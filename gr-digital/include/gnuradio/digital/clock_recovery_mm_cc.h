/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_H
#define	INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Mueller and Müller (M&M) based clock recovery block with complex input, complex output.
     * \ingroup synchronizers_blk
     *
     * \details
     * This implements the Mueller and Müller (M&M) discrete-time
     * error-tracking synchronizer.
     *
     * The complex version here is based on: Modified Mueller and
     * Muller clock recovery circuit:
     *
     *    G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller
     *    and Muller algorithm," Electronics Letters, Vol. 31, no. 13, 22
     *    June 1995, pp. 1032 - 1033.
     */
    class DIGITAL_API clock_recovery_mm_cc : virtual public block
    {
    public:
      // gr::digital::clock_recovery_mm_cc::sptr
      typedef boost::shared_ptr<clock_recovery_mm_cc> sptr;

      /*!
       * Make a M&M clock recovery block.
       *
       * \param omega Initial estimate of samples per symbol
       * \param gain_omega Gain setting for omega update loop
       * \param mu Initial estimate of phase of sample
       * \param gain_mu Gain setting for mu update loop
       * \param omega_relative_limit limit on omega
       */
      static sptr make(float omega, float gain_omega,
		       float mu, float gain_mu,
		       float omega_relative_limit);

      virtual float mu() const = 0;
      virtual float omega() const = 0;
      virtual float gain_mu() const = 0;
      virtual float gain_omega() const = 0;

      virtual void set_verbose(bool verbose) = 0;
      virtual void set_gain_mu (float gain_mu) = 0;
      virtual void set_gain_omega (float gain_omega) = 0;
      virtual void set_mu (float mu) = 0;
      virtual void set_omega (float omega) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_CC_H */
