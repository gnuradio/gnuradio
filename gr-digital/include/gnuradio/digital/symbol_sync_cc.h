/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_SYMBOL_SYNC_CC_H
#define INCLUDED_DIGITAL_SYMBOL_SYNC_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>
#include <gnuradio/digital/timing_error_detector_type.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/digital/interpolating_resampler_type.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Symbol Synchronizer block with complex input, complex output.
     * \ingroup synchronizers_blk
     *
     * \details
     * This implements a discrete-time error-tracking synchronizer.
     *
     * For this block to work properly, the input stream must meet the
     * following requirements:
     *
     * 1. if not using the PFB Matched Filter interpolator, and using
     * a non-CPM timing error detector, the input pulses must have peaks
     * (not flat), which usually can be implemented by using a matched
     * filter before this block.
     *
     * 2. for decision directed timing error detectors, the input pulses
     * should nominally match the normalized slicer constellation, which
     * is normalized to an average symbol magnitude of 1.0 over the entire
     * constellation.
     */
    class DIGITAL_API symbol_sync_cc : virtual public block
    {
    public:
      // gr::digital::symbol_sync_cc::sptr
      typedef boost::shared_ptr<symbol_sync_cc> sptr;

      /*!
       * Make a Symbol Synchronizer block.
       *
       * \details
       * This implements a discrete-time error-tracking synchronizer.
       *
       * For this block to work properly, the input stream must meet the
       * following requirements:
       *
       * 1. if not using the PFB Matched Filter interpolator, and using
       * a non-CPM timing error detector, the input pulses must have peaks
       * (not flat), which usually can be implemented by using a matched
       * filter before this block.
       *
       * 2. for decision directed timing error detectors, the input pulses
       * should nominally match the normalized slicer constellation, which
       * is normalized to an average symbol magnitude of 1.0 over the entire
       * constellation.
       *
       * \param detector_type
       * The enumerated type of timing error detector to use.
       * See enum ted_type for a list of possible types.
       *
       * \param sps
       * User specified nominal clock period in samples per symbol.
       *
       * \param loop_bw
       * Approximate normailzed loop bandwidth of the symbol clock tracking
       * loop. It should nominally be close to 0, but greater than 0.  If
       * unsure, start with a number around 2*pi*0.040, and experiment to find
       * the value that works best for your situation.
       *
       * \param damping_factor
       * Damping factor of the symbol clock tracking loop.
       * Damping < 1.0f is an under-damped loop.
       * Damping = 1.0f/sqrt(2.0f) is a maximally flat loop response.
       * Damping = 1.0f is a critically-damped loop.
       * Damping > 1.0f is an over-damped loop.
       *
       * \param ted_gain
       * Expected gain of the timing error detector, given the TED in use
       * and the anticipated input amplitude, pulse shape, and Es/No.
       * This value is the slope of the TED's S-curve at timing offset tau = 0.
       * This value is normally computed by the user analytically or by
       * simulation in a tool outside of GNURadio.
       * This value must be correct for the loop filter gains to be computed
       * properly from the desired input loop bandwidth and damping factor.
       *
       * \param max_deviation
       * Maximum absolute deviation of the average clock period estimate
       * from the user specified nominal clock period in samples per symbol.
       *
       * \param osps
       * The number of output samples per symbol (default=1).
       *
       * \param slicer
       * A constellation obj shared pointer that will be used by
       * decision directed timing error detectors to make decisions.
       * I.e. the timing error detector will use this constellation
       * as a slicer, if the particular algorithm needs sliced
       * symbols.
       *
       * \param interp_type
       * The enumerated type of interpolating resampler to use.
       * See the interpolating resampler type enum for a list of possible types.
       *
       * \param n_filters
       * The number of arms in the polyphase filterbank of the interpolating
       * resampler, if using an interpolating resampler that uses a PFB.
       *
       * \param taps
       * The prototype filter for the polyphase filterbank of the interpolating
       * resampler, if using an interpolating resampler that uses a PFB.
       */
      static sptr make(enum ted_type detector_type,
                       float sps,
                       float loop_bw,
                       float damping_factor = 1.0f,
                       float ted_gain = 1.0f,
                       float max_deviation = 1.5f,
                       int osps = 1,
                       constellation_sptr slicer = constellation_sptr(),
                       ir_type interp_type = IR_MMSE_8TAP,
                       int n_filters = 128,
                       const std::vector<float> &taps = std::vector<float>());

      /*!
       * \brief Returns the normalized approximate loop bandwidth.
       *
       * \details
       * See the documentation for set_loop_bandwidth() for more details.
       *
       * Note that if set_alpha() or set_beta() were called to directly
       * set gains, the value returned by this method will be inaccurate/stale.
       */
      virtual float loop_bandwidth() const = 0;

      /*!
       * \brief Returns the loop damping factor.
       *
       * \details
       * See the documentation for set_damping_factor() for more details.
       *
       * Note that if set_alpha() or set_beta() were called to directly
       * set gains, the value returned by this method will be inaccurate/stale.
       */
      virtual float damping_factor() const = 0;

      /*!
       * \brief Returns the user provided expected gain of the Timing Error
       * Detector.
       *
       * \details
       * See the documentation for set_ted_gain() for more details.
       */
      virtual float ted_gain() const = 0;

      /*!
       * \brief Returns the PI filter proportional gain, alpha.
       *
       * \details
       * See the documentation for set_alpha() for more details.
       */
      virtual float alpha() const = 0;

      /*!
       * \brief Returns the PI filter integral gain, beta.
       *
       * \details
       * See the documentation for set_beta() for more details.
       */
      virtual float beta() const = 0;

      /*!
       * \brief Set the normalized approximate loop bandwidth.
       *
       * \details
       * Set the normalized approximate loop bandwidth.
       * Useful values are usually close to 0.0, e.g. 2*pi*0.045.
       *
       * It should be a small positive number, corresponding to the normalized
       * natural radian frequency of the loop as digital low-pass filter that is
       * filtering the clock phase/timing error.
       *
       * Technically this parameter corresponds to the natural radian frequency
       * of the 2nd order loop transfer function (scaled by Fs),
       * which is the radius of the pole locations in the s-plane of an
       * underdamped analog 2nd order system.
       *
       * The input parameter corresponds to omega_n_norm in the following
       * relation:
       *
       *     omega_n_norm = omega_n*T = 2*pi*f_n*T = 2*pi*f_n_norm
       *
       * where T is the period of the clock being estimated by this
       * clock tracking loop, and omega_n is the natural radian frequency
       * of the 2nd order loop transfer function.
       *
       * When a new loop bandwidth is set, the gains, alpha and beta,
       * of the loop are automatically recalculated.
       *
       * \param omega_n_norm    normalized approximate loop bandwidth
       */
      virtual void set_loop_bandwidth (float omega_n_norm) = 0;

      /*!
       * \brief Set the loop damping factor.
       *
       * \details
       * Set the damping factor of the loop.
       * Damping in the range (0.0, 1.0) yields an under-damped loop.
       * Damping in the range (1.0, Inf) yields an over-damped loop.
       * Damping equal to 1.0 yields a crtically-damped loop.
       * Damping equal to 1.0/sqrt(2.0) yields a maximally flat
       * loop filter response.
       *
       * Damping factor of the 2nd order loop transfer function.
       * When a new damping factor is set, the gains, alpha and beta,
       * of the loop are automatcally recalculated.
       *
       * \param zeta    loop damping factor
       */
      virtual void set_damping_factor (float zeta) = 0;

      /*!
       * \brief Set the expected gain of the Timing Error Detector.
       *
       * \details
       * Sets the expected gain of the timing error detector, given the TED in
       * use and the anticipated input amplitude, pulse shape, and Es/No.
       * This value is the slope of the TED's S-curve at timing offset tau = 0.
       * This value is normally computed by the user analytically or by
       * simulation in a tool outside of GNURadio.
       * This value must be correct for the loop filter gains to be computed
       * properly from the desired input loop bandwidth and damping factor.
       *
       * When a new ted_gain is set, the gains, alpha and beta,
       * of the loop are automatcally recalculated.
       *
       * \param ted_gain    expected gain of the timing error detector
       */
      virtual void set_ted_gain (float ted_gain) = 0;

      /*!
       * \brief Set the PI filter proportional gain, alpha.
       *
       * \details
       * Sets the PI filter proportional gain, alpha.
       * This gain directly mutliplies the clock phase/timing error
       * term in the PI filter when advancing the loop.
       * It most directly affects the instantaneous clock period estimate,
       * T_inst, and instantaneous clock phase estimate, tau.
       *
       * This value would normally be adjusted by setting the loop
       * bandwidth and damping factor. However,
       * it can be set here directly if desired.
       *
       * Setting this parameter directly is probably only feasible if
       * the user is directly observing the estimates of average clock
       * period and instantaneous clock period over time in response to
       * an impulsive change in the input stream (i.e. watching the loop
       * transient behavior at the start of a data burst).
       *
       * \param alpha    PI filter proportional gain
       */
      virtual void set_alpha (float alpha) = 0;

      /*!
       * \brief Set the PI filter integral gain, beta.
       *
       * \details
       * Sets the PI filter integral gain, beta.
       * This gain is used when integrating the clock phase/timing error
       * term in the PI filter when advancing the loop.
       * It most directly affects the average clock period estimate,
       * T_avg.
       *
       * This value would normally be adjusted by setting the loop
       * bandwidth and damping factor. However,
       * it can be set here directly if desired.
       *
       * Setting this parameter directly is probably only feasible if
       * the user is directly observing the estimates of average clock
       * period and instantaneous clock period over time in response to
       * an impulsive change in the input stream (i.e. watching the loop
       * transient behavior at the start of a data burst).
       *
       * \param beta    PI filter integral gain
       */
      virtual void set_beta (float beta) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_SYMBOL_SYNC_CC_H */
