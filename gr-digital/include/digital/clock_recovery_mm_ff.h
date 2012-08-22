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

#ifndef INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_H
#define	INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_H

#include <digital/api.h>
#include <gr_block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Mueller and Müller (M&M) based clock recovery block with float input, float output.
     * \ingroup sync_blk
     * \ingroup digital
     *
     * This implements the Mueller and Müller (M&M) discrete-time
     * error-tracking synchronizer.
     *
     * See "Digital Communication Receivers: Synchronization, Channel
     * Estimation and Signal Processing" by Heinrich Meyr, Marc
     * Moeneclaey, & Stefan Fechtel.  ISBN 0-471-50275-8.
     */
    class DIGITAL_API clock_recovery_mm_ff : virtual public gr_block
    {
    public:
      // gr::digital::clock_recovery_mm_ff::sptr
      typedef boost::shared_ptr<clock_recovery_mm_ff> sptr;

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

#endif /* INCLUDED_DIGITAL_CLOCK_RECOVERY_MM_FF_H */
