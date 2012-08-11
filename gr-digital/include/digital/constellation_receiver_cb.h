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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H
#define	INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H

#include <digital/api.h>
#include <digital/constellation.h>
#include <gr_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief This block takes care of receiving generic modulated signals
     * through phase, frequency, and symbol synchronization.
     * \ingroup sync_blk
     * \ingroup demod_blk
     * \ingroup digital
     *
     * This block takes care of receiving generic modulated signals
     * through phase, frequency, and symbol synchronization. It
     * performs carrier frequency and phase locking as well as symbol
     * timing recovery.
     *
     * The phase and frequency synchronization are based on a Costas
     * loop that finds the error of the incoming signal point compared
     * to its nearest constellation point. The frequency and phase of
     * the NCO are updated according to this error.
     *
     * The symbol synchronization is done using a modified Mueller and
     * Muller circuit from the paper:
     * 
     * "G. R. Danesfahani, T.G. Jeans, "Optimisation of modified
     * Mueller and Muller algorithm," Electronics Letters, Vol. 31,
     * no. 13, 22 June 1995, pp. 1032 - 1033."
     *
     * This circuit interpolates the downconverted sample (using the
     * NCO developed by the Costas loop) every mu samples, then it
     * finds the sampling error based on this and the past symbols and
     * the decision made on the samples. Like the phase error
     * detector, there are optimized decision algorithms for BPSK and
     * QPKS, but 8PSK uses another brute force computation against all
     * possible symbols. The modifications to the M&M used here reduce
     * self-noise.
     */
    class DIGITAL_API constellation_receiver_cb
      : virtual public gr_block
    {
    public:
      // gr::digital::constellation_receiver_cb::sptr
      typedef boost::shared_ptr<constellation_receiver_cb> sptr;

      /*!
       * \brief Constructor to synchronize incoming M-PSK symbols
       *
       * \param constellation constellation of points for generic modulation
       * \param loop_bw	Loop  bandwidth of the Costas Loop (~ 2pi/100)
       * \param fmin          minimum normalized frequency value the loop can achieve
       * \param fmax          maximum normalized frequency value the loop can achieve
       *
       * The constructor chooses which phase detector and decision
       * maker to use in the work loop based on the value of M.
       */
      static sptr make(constellation_sptr constellation, 
		       float loop_bw, float fmin, float fmax);

      virtual void phase_error_tracking(float phase_error) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_H */
