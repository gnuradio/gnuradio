/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_MPSK_RECEIVER_CC_H
#define	INCLUDED_DIGITAL_MPSK_RECEIVER_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/control_loop.h>

namespace gr {
  namespace digital {

    /*!
     * \brief This block takes care of receiving M-PSK modulated
     * signals through phase, frequency, and symbol synchronization.
     * \ingroup synchronizers_blk
     * \ingroup deprecated_blk
     *
     * \details
     * It performs carrier frequency and phase locking as well as
     * symbol timing recovery.  It works with (D)BPSK, (D)QPSK, and
     * (D)8PSK as tested currently. It should also work for OQPSK and
     * PI/4 DQPSK.
     *
     * The phase and frequency synchronization are based on a Costas
     * loop that finds the error of the incoming signal point compared
     * to its nearest constellation point. The frequency and phase of
     * the NCO are updated according to this error. There are
     * optimized phase error detectors for BPSK and QPSK, but 8PSK is
     * done using a brute-force computation of the constellation
     * points to find the minimum.
     *
     * The symbol synchronization is done using a modified Mueller and
     * Muller circuit from the paper:
     *
     * "G. R. Danesfahani, T. G. Jeans, "Optimisation of modified Mueller
     * and Muller algorithm," Electronics Letters, Vol. 31, no. 13, 22
     * June 1995, pp. 1032 - 1033."
     *
     * This circuit interpolates the downconverted sample (using the
     * NCO developed by the Costas loop) every mu samples, then it
     * finds the sampling error based on this and the past symbols and
     * the decision made on the samples. Like the phase error
     * detector, there are optimized decision algorithms for BPSK and
     * QPKS, but 8PSK uses another brute force computation against all
     * possible symbols. The modifications to the M&M used here reduce
     * self-noise.
     *
     */
    class DIGITAL_API mpsk_receiver_cc
      : virtual public block,
        virtual public blocks::control_loop
    {
    public:
      // gr::digital::mpsk_receiver_cc::sptr
      typedef boost::shared_ptr<mpsk_receiver_cc> sptr;

      /*!
       * \brief Make a M-PSK receiver block.
       *
       * \param M	   modulation order of the M-PSK modulation
       * \param theta	   any constant phase rotation from the real axis of the constellation
       * \param loop_bw	   Loop bandwidth to set gains of phase/freq tracking loop
       * \param fmin       minimum normalized frequency value the loop can achieve
       * \param fmax       maximum normalized frequency value the loop can achieve
       * \param mu         initial parameter for the interpolator [0,1]
       * \param gain_mu    gain parameter of the M&M error signal to adjust mu (~0.05)
       * \param omega      initial value for the number of symbols between samples (~number of samples/symbol)
       * \param gain_omega gain parameter to adjust omega based on the error (~omega^2/4)
       * \param omega_rel  sets the maximum (omega*(1+omega_rel)) and minimum (omega*(1+omega_rel)) omega (~0.005)
       *
       * The constructor also chooses which phase detector and
       * decision maker to use in the work loop based on the value of
       * M.
       */
      static sptr make(unsigned int M, float theta,
		       float loop_bw,
		       float fmin, float fmax,
		       float mu, float gain_mu,
		       float omega, float gain_omega, float omega_rel);

      //! Returns the modulation order (M) currently set
      virtual float modulation_order() const = 0;

      //! Returns current value of theta
      virtual float theta() const = 0;

      //! Returns current value of mu
      virtual float mu() const = 0;

      //! Returns current value of omega
      virtual float omega() const = 0;

      //! Returns mu gain factor
      virtual float gain_mu() const = 0;

      //! Returns omega gain factor
      virtual float gain_omega() const = 0;

      //! Returns the relative omega limit
      virtual float gain_omega_rel() const = 0;

      //! Sets the modulation order (M) currently
      virtual void set_modulation_order(unsigned int M) = 0;

      //! Sets value of theta
      virtual void set_theta(float theta) = 0;

      //! Sets value of mu
      virtual void set_mu(float mu) = 0;

      //! Sets value of omega and its min and max values
      virtual void set_omega(float omega) = 0;

      //! Sets value for mu gain factor
      virtual void set_gain_mu(float gain_mu) = 0;

      //! Sets value for omega gain factor
      virtual void set_gain_omega(float gain_omega) = 0;

      //! Sets the relative omega limit and resets omega min/max values
      virtual void set_gain_omega_rel(float omega_rel) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_MPSK_RECEIVER_CC_H */
