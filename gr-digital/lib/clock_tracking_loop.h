/* -*- c++ -*- */
/*
 * Copyright (C) 2011,2013,2016-2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CLOCK_TRACKING_LOOP_H
#define INCLUDED_DIGITAL_CLOCK_TRACKING_LOOP_H

namespace gr {
  namespace digital {

    /*!
     * \brief A second-order control loop implementation class.
     *
     * \details
     * This class implements most of a second order symbol clock
     * tracking loop and is inteded to act as a parent class to blocks
     * which need a symbol clock tracking loop to determine the optimal
     * instant to sample a received symbol from an input sample
     * stream (i.e. *_clock_recovery* and *_clock_sync* blocks).
     * It takes in an expected timing error detector gain, a normalized loop
     * bandwidth and damping factor, as well as clock period bounds, and
     * provides the functions that control the update of the loop.
     *
     * This control loop runs at the rate of the output clock, so
     * each step of the loop produces estimates about the output clock,
     * and the clock phase/timing error input must come at a rate of
     * once per output clock.
     *
     * This class does not include a timing error detector, and the 
     * caller is expected to provide the clock phase/timing error input
     * for each step of the loop.
     *
     * The loop's low pass filter is a Proportional-Integral (PI) filter.
     * The proportional and integral gains of the filter are termed alpha
     * and beta respectively. These gains are calculated using the input
     * expected timing error detector gain, loop bandwidth and damping factor.
     * If needed, the alpha and beta gain values can be set using their
     * respective #set_alpha or #set_beta functions.
     *
     * The class estimates the average clock period, T_avg; the instantaneous
     * clock period, T_inst; and the instantaneous clock phase, tau; of a
     * symbol clock based on an error signal from an external clock phase/
     * timing error detector which provides one error signal sample per
     * clock (one error sample at the end of every T_inst clock cycle).
     * The error calculation is unique for each TED algorithm and is
     * calculated externally and passed to the advance_loop function,
     * which uses this to update its estimates.
     *
     * This class also provides the functions #phase_wrap and
     * #period_limit to easily keep the clock phase estimate, tau, and the
     * average clock period estimate, T_avg, within set bounds (phase_wrap
     * keeps the phase within +/-T_avg/2).
     *
     * The clock tracking loop, with its PI filter, when properly implemented, has
     * a digital loop phase-transfer function, in terms of the timing error
     * detector gain, \f$K_{ted}\f$; proportional gain, \f$\alpha\f$; and the
     * integral gain, \f$\beta\f$, as follows:
     * 
     * \f{align*}
     *    H(z) &= \dfrac {\Theta_o(z)}{\Theta_i(z)}
     *          = K_{ted}(\alpha + \beta)z^{-1} \cdot
     *            \dfrac{
     *                   1
     *                   - \dfrac{\alpha}{\alpha + \beta} z^{-1}
     *                  }
     *                  {
     *                   1
     *                   - 2 \left(1 - K_{ted}\dfrac{\alpha + \beta}{2}\right) z^{-1}
     *                   + (1 - K_{ted}\alpha) z^{-2}
     *                  } \\
     * \f}
     *
     * Mapping the above phase-transfer function to the standard form of a transfer
     * function for an analog second order control loop mapped to the digital domain
     * with the mapping \f$z = e^{sT}\f$ applied to the s-plane poles,
     * \f$s_{1,2} = -\zeta\omega_{n} \pm \omega_{n}\sqrt{\zeta^{2}-1}\f$, one obtains an
     * alternate form of the transfer function, directly related to the damping factor
     * \f$\zeta\f$, the natural radian frequency \f$\omega_{n}\f$, the damped radian frequency
     * of oscillation \f$\omega_{d}\f$, and the symbol clock period \f$T\f$:
     * 
     * \f{align*}
     *   H(z) &=
     *      \begin{cases}
     *         \dfrac{
     *                [2 -2\cos(\omega_{d}T)e^{-\zeta\omega_{n}T}] z 
     *                -2\sinh(\zeta\omega_{n}T)e^{-\zeta\omega_{n}T} 
     *               }
     *               {
     *                z^{2}
     *                - 2 \cos(\omega_{d}T) e^{-\zeta\omega_{n}T} z
     *                + e^{-2\zeta\omega_{n}T}
     *               }
     *               & \quad \text{for} \quad \zeta < 1 \quad \text{with}
     *               \quad \omega_{d}T = \omega_{n}T \sqrt{1 - \zeta^{2}}
     *               \\
     * \\
     *         \dfrac{
     *                [2 -2(1)e^{-\zeta\omega_{n}T}] z 
     *                -2\sinh(\zeta\omega_{n}T)e^{-\zeta\omega_{n}T} 
     *               }
     *               {
     *                z^{2}
     *                - 2(1)e^{-\zeta\omega_{n}T} z
     *                + e^{-2\zeta\omega_{n}T}
     *               }
     *               & \quad \text{for} \quad \zeta = 1 \quad \text{with}
     *               \quad \omega_{d}T = 0
     *               \\
     * \\
     *         \dfrac{
     *                [2 -2\cosh(\omega_{d}T)e^{-\zeta\omega_{n}T}] z 
     *                -2\sinh(\zeta\omega_{n}T)e^{-\zeta\omega_{n}T} 
     *               }
     *               {
     *                z^{2}
     *                - 2 \cosh(\omega_{d}T) e^{-\zeta\omega_{n}T} z
     *                + e^{-2\zeta\omega_{n}T}
     *               }
     *               & \quad \text{for} \quad \zeta > 1 \quad \text{with}
     *               \quad \omega_{d}T = \omega_{n}T \sqrt{\zeta^{2} - 1}
     *               \\
     *      \end{cases}
     * \\
     * \f}
     * 
     * The PI filter gains, expressed in terms of the damping factor, \f$\zeta\f$;
     * the natural radian frequency, \f$\omega_{n}\f$; the damped radian frequency of
     * oscillation, \f$\omega_{d}\f$; the timing error detector gain \f$K_{ted}\f$;
     * and the clock period \f$T\f$ are:
     * 
     * \f{align*}
     *   \alpha &= \dfrac{2}{K_{ted}}e^{-\zeta\omega_{n}T} \sinh(\zeta\omega_{n}T) \\
     * \\
     *   \beta  &=
     *      \begin{cases}
     *         \dfrac{2}{K_{ted}} \left(1 -
     *          e^{-\zeta\omega_{n}T} [\sinh(\zeta\omega_{n}T) + \cos(\omega_{d}T)]
     *          \right) &
     *         \text{for} \quad \zeta < 1 \quad (under \: damped)\\ \\
     *         \dfrac{2}{K_{ted}} \left(1 -
     *          e^{-\zeta\omega_{n}T} [\sinh(\zeta\omega_{n}T) + 1]
     *          \right) &
     *         \text{for} \quad \zeta = 1 \quad (critically \: damped)\\ \\
     *         \dfrac{2}{K_{ted}} \left(1 -
     *          e^{-\zeta\omega_{n}T} [\sinh(\zeta\omega_{n}T) +\cosh(\omega_{d}T)]
     *          \right) &
     *         \text{for} \quad \zeta > 1 \quad (over \: damped)\\
     *      \end{cases} \\
     * \\
     * \f}
     * 
     * It should be noted that the clock period \f$T\f$ is being estimated by the clock
     * tracking loop and can vary over time, so that setting the loop bandwidth
     * directly can be a problem.  However, we specify loop bandwidth in terms
     * of the normalized digital natural radian frequency \f$\omega_{n\_norm}\f$ of the loop.
     * \f$\omega_{n\_norm}\f$ can only usefully be a small positive number, close to
     * zero.  The damping factor, \f$\zeta\f$, dictates the maximum value
     * \f$\omega_{n\_norm}\f$ can practically take on.  In the extreme a case of
     * \f$\zeta = 0.0\f$, \f$\omega_{n\_norm}\f$ is practically limited to the range
     * \f$(0, \pi)\f$, as \f$\pi\f$ then corresponds to the Nyquist frequency
     * of the clock.  However, whatever the damping factor, large values of
     * \f$\omega_{n\_norm}\f$ are usually not useful and yield poor results.
     * 
     * \f{align*}
     *     \omega_{n}T = \omega_{n\_norm} = 2 \pi f_{n\_norm} = 2 \pi f_{n} T =
     *     \pi \dfrac{f_{n}}{\left(\dfrac{F_{c}}{2}\right)}
     * \f}
     * 
     * In practice, the timing error detector (TED) of the symbol clock
     * tracking loop is implemented with an estimator of symbol clock phase
     * error, which has some gain \f$K_{ted}\f$.  The gain, \f$K_{ted}\f$, is
     * defined as the slope of a TED's S-curve plot at a symbol clock phase
     * offset of \f$\tau = 0\f$.  The S-curve shape and central slope, and
     * hence the gain \f$K_{ted}\f$, depend on the TED's estimator espression,
     * the input signal level, the pulse shaping filter, and the \f$E_s/N_0\f$
     * of the incomping signal.  The user must determine the TED's
     * S-curve by analysis or simulation of the particular situation, in order
     * to determine an appropriate value for \f$K_{ted}\f$.
     *
     * * A note on symbol clock phase vs. interpolating resampler sample phase,
     * since most GNURadio symbol synchronization blocks seem to have the same
     * implementation error:
     *
     * In general, the symbol clock phase, that the symbol clock tracking loop
     * estimates and tracks, cannot be used alone to derive the interpolating resampler
     * sample phase used in symbol synchronization, except in the very special case of
     * the symbol clock period being exactly divisible by the input sample stream
     * sample period.  Since this is never guaranteed in tracking real symbol clocks,
     * one should not use the symbol clock phase alone to compute the interpolating
     * resampler sample phase.
     *
     * Consider, in the analog time domain, the optimum symbol sampling instants
     * \f$t_{k}\f$, of an analog input signal \f$x(t)\f$, at an optimal symbol clock
     * phase \f$\tau_{0}\f$ and the symbol clock period \f$T_{c}\f$:
     * 
     * \f{align*}
     *    t_{k} &= \tau_{0} + k T_{c} \\
     *    y_{k} &= x(t_{k}) = x(\tau_{0} + k T_{c}) \\
     * \f}
     * 
     * If one divides the \f$t_{k}\f$ times by the input sample stream sample period
     * \f$T_{i}\f$, the correct interpolating resampler sample phase \f$\tau_{0\_i}\f$ will
     * get a contribution from the term \f$T_{c\_remainder}\f$ (which is an error) as shown
     * below:
     * 
     * \f{align*}
     *    \dfrac{t_{k}}{T_{i}} &= \dfrac{\tau_{0}}{T_{i}} + \dfrac{k T_{c}}{T_{i}} \\
     *     &= (m + \tau_{0\_remainder}) + (n + T_{c\_remainder}) \\
     *     &= \tau_{0\_remainder} + T_{c\_remainder} + (m + n) \\
     *     &= \tau_{0\_i} + k^{\prime}
     * \f}
     * 
     * So instead of using the symbol clock sample phase alone to obtain the
     * interpolating resampler sample phase, one should use the previous interpolating
     * resampler sample phase and the instantaneous clock period estimate provided by
     * the symbol clock tracking loop.
     *
     */
    class clock_tracking_loop
    {
    protected:
      // Estimate of the average clock period, T_avg, in units of
      // input sample clocks (so this is the average number of
      // input samples per output symbol, aka samples/symbol).
      // To convert to seconds, divide by the input sample rate: F_s_input.
      float d_avg_period;

      // Limits on how far the average clock period estimate can wander,
      // and the nominal average clock period, in units of input sample clocks.
      // To convert to seconds, divide by the input sample rate: F_s_input.
      float d_max_avg_period, d_min_avg_period;
      float d_nom_avg_period;

      // Instantaneous clock period estimate, T_inst, in units of
      // input sample clocks (so this is the intantaneous number of
      // input samples per output symbol, aka instantaneous samples/symbol).
      // To convert to seconds, divide by the input sample rate: F_s_input.
      float d_inst_period;

      // Instantaneous clock phase estimate, tau, in units of
      // input sample clocks.
      // To convert to seconds, divide by the input sample rate: F_s_input.
      // To wrap, add or subtract a multiple of the estimate of the 
      // average clock period, T_avg.
      // To convert to a normalized (but not wrapped) clock phase estimate,
      // divide by the estimate of the average clock period, T_avg.  
      // To further convert the normalized clock phase estimate to radians,
      // multiply the normalized clock phase estimate by 2*pi.
      float d_phase;

      // Damping factor of the 2nd order loop transfer function.
      // Zeta in the range (0.0, 1.0) yields an under-damped loop.
      // Zeta in the range (1.0, Inf) yields an over-damped loop.
      // Zeta equal to 1.0 yields a crtically-damped loop.
      float d_zeta;

      // Normalized natural radian frequency of the 2nd order loop transfer
      // function.  It should be a small positive number, corresponding to
      // the normalized natural radian frequency of the loop as digital
      // low-pass filter that is filtering the clock phase/timing error signal.
      // omega_n_norm = omega_n*T  = 2*pi*f_n*T = 2*pi*f_n_norm
      float d_omega_n_norm;

      // Expected gain of the timing error detector in use, given the
      // TED estimator expression, the expected input amplitude, the
      // input pulse shape, and the expected input Es/No.  (This value is the
      // slope of the TED's S-curve plot at a timing offset of tau = 0, and
      // must be determined by analysis and/or simulation by the user.)
      float d_ted_gain;

      // Proportional gain of the PI loop filter (aka gain_mu)
      // (aka gain_mu in some clock recovery blocks)
      float d_alpha;

      // Integral gain of the PI loop filter
      // (aka gain_omega in some clock recovery blocks)
      float d_beta;

      // For reverting the loop state one iteration (only)
      float d_prev_avg_period;
      float d_prev_inst_period;
      float d_prev_phase;

    public:
      clock_tracking_loop(void) {}

      /*! \brief Construct a clock_tracking_loop object.
       *
       * \details
       * This function instantiates a clock_tracking_loop object. 
       *
       * \param loop_bw
       * Normalized approximate loop bandwidth.
       * It should be a small positive number, corresponding to the normalized
       * natural radian frequency of the loop as digital low-pass filter that is
       * filtering the clock phase/timing error.
       *
       * Technically this parameter corresponds to the natural radian frequency
       * of the 2nd order loop transfer function (scaled by Fs),
       * which is the radius of the pole locations in the s-plane of an
       * underdamped analog 2nd order system.
       *
       * \param max_period
       * Maximum limit for the estimated clock period, in units of
       * input stream sample periods. (i.e. maximum samples/symbol)
       *
       * \param min_period
       * Minimum limit for the estimated clock period, in units of
       * input stream sample periods. (i.e. minimum samples/symbol)
       *
       * \param nominal_period
       * Nominal value for the estimated clock period, in units of
       * input stream sample periods. (i.e. nominal samples/symbol)
       * If not specified, this value will be set to the average of
       * min_period and max_period,
       *
       * \param damping
       * Damping factor of the 2nd order loop transfer function.
       * Damping in the range (0.0, 1.0) yields an under-damped loop.
       * Damping in the range (1.0, Inf) yields an over-damped loop.
       * Damping equal to 1.0 yields a crtically-damped loop.
       * Under-damped loops are not generally useful for clock tracking.
       * This parameter defaults to 1.0, if not specified.
       *
       * \param ted_gain
       * Expected gain of the timing error detector, given the TED in use
       * and the anticipated input amplitude, pulse shape, and Es/No.
       * This value is the slope of the TED's S-curve at timing offset tau = 0.
       * This value is normally computed by the user analytically or by
       * simulation in a tool outside of GNURadio.
       * This value must be correct for the loop filter gains to be computed
       * properly from the desired input loop bandwidth and damping factor.
       * This parameter defaults to 1.0, if not specified.
       */
      clock_tracking_loop(float loop_bw,
                          float max_period, float min_period,
                          float nominal_period = 0.0f,
                          float damping = 1.0f,
                          float ted_gain = 1.0f);

      virtual ~clock_tracking_loop();

      /*! \brief Update the gains from the ted_gain, loop bw and damping factor.
       *
       * \details
       * This function updates the gains based on the loop
       * bandwidth and damping factor of the system. These two
       * factors can be set separately through their own set
       * functions.
       */
      void update_gains();

      /*! \brief Advance the loop based on the current gain
       *  settings and the input error signal.
       */
      void advance_loop(float error);

      /*! \brief Undo a single, prior advance_loop() call.
       *
       * \details
       * Reverts a single, prior call to advance_loop().
       * It cannot usefully called again, until after the next call
       * to advance_loop().
       *
       * This method is needed so clock recovery/sync blocks can
       * perform correctly given the constraints of GNURadio's streaming
       * engine, interpolation filtering, and tag propagation.
       */
      void revert_loop();

      /*! \brief
       * Keep the clock phase estimate, tau, between -T_avg/2 and T_avg/2.
       *
       * \details
       * This function keeps the clock phase estimate, tau, between
       * -T_avg/2 and T_avg/2, by wrapping it modulo the estimated
       * average clock period, T_avg.  (N.B. Wrapping an estimated phase
       * by an *estimated*, *average* period.)
       *
       * This function can be called after advance_loop to keep the
       * phase value small.  It is set as a separate method in case
       * another way is desired as this is fairly heavy-handed.
       * Clock recovery/sync blocks usually do not need the phase of the
       * clock, and this class doesn't actually use the phase at all,
       * so calling this is optional.
       */
      void phase_wrap();

      /*! \brief
       * Keep the estimated average clock period, T_avg, between T_avg_min
       * and T_avg_max.
       *
       * \details
       * This function keeps the estimated average clock period, T_avg,
       * between T_avg_min and T_avg_max. It accomplishes this by hard limiting.
       * This is needed because T_avg is essentially computed by the
       * integrator portion of an IIR filter, so T_avg could potentially
       * wander very far during periods of noise/nonsense input.
       *
       * This function should be called after advance_loop to keep the
       * estimated average clock period, T_avg, in the specified range.
       * It is set as a separate method in case another way is desired as
       * this is fairly heavy-handed.
       */
      void period_limit();

      /*******************************************************************
       * SET FUNCTIONS
       *******************************************************************/

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
       * of the loop are recalculated by a call to update_gains().
       *
       * \param bw    normalized approximate loop bandwidth
       */
      virtual void set_loop_bandwidth(float bw);

      /*!
       * \brief Set the loop damping factor.
       *
       * \details
       * Set the damping factor of the loop.
       * Damping in the range (0.0, 1.0) yields an under-damped loop.
       * Damping in the range (1.0, Inf) yields an over-damped loop.
       * Damping equal to 1.0 yields a crtically-damped loop.
       * Under-damped loops are not generally useful for clock tracking.
       * For clock tracking, as a first guess, set the damping factor to 2.0,
       * or 1.5 or 1.0.
       *
       * Damping factor of the 2nd order loop transfer function.
       * When a new damping factor is set, the gains, alpha and beta,
       * of the loop are recalculated by a call to update_gains().
       *
       * \param df    loop damping factor
       */
      void set_damping_factor(float df);

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
      void set_ted_gain (float ted_gain);

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
       * bandwidth and damping factor and calling update_gains(). However,
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
      void set_alpha(float alpha);

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
       * bandwidth and damping factor and calling update_gains(). However,
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
      void set_beta(float beta);

      /*!
       * \brief Set the average clock period estimate, T_avg.
       *
       * \details
       * Directly sets the average clock period estimate, T_avg,
       * in units of input stream sample clocks (so the average number of
       * input samples per output symbol, aka samples/symbol).
       *
       * The average clock period estimate, T_avg, is normally updated by
       * the advance_loop() and period_limit() calls. This method is used
       * manually reset the estimate when needed.
       *
       * \param period
       * Average clock period, T_avg, in units of input stream sample clocks.
       */
      void set_avg_period(float period);

      /*!
       * \brief Set the instantaneous clock period estimate, T_inst.
       *
       * \details
       * Directly sets the instantaneous clock period estimate, T_inst,
       * in units of input stream sample clocks (so the instantaneous number of
       * input samples per output symbol, aka instantaneous samples/symbol).
       *
       * The instantaneous clock period estimate, T_inst, is normally updated by
       * the advance_loop() call. This method is used manually reset the
       * estimate when needed.
       *
       * \param period
       * Instantaneous clock period, T_inst, in units of input stream sample
       * clocks.
       */
      void set_inst_period(float period);

      /*!
       * \brief Set the instantaneous clock phase estimate, tau.
       *
       * \details
       * Directly sets the instantaneous clock phase estimate, tau,
       * in units of input stream sample clocks.
       *
       * The instantaneous clock phase estimate, tau, is normally updated by
       * the advance_loop() call. This method is used manually reset the
       * estimate when needed.
       *
       * \param phase 
       * Instantaneous clock phase, tau, in units of input stream sample clocks.
       *
       */
      void set_phase(float phase);

      /*!
       * \brief Set the maximum average clock period estimate limit, T_avg_max.
       *
       * \details
       * Sets the maximum average clock period estimate limit, T_avg_max
       * in units of input stream sample clocks (so the maximum average number
       * of input samples per output symbol, aka maximum samples/symbol).
       *
       * This limit is needed because T_avg is essentially computed by the
       * integrator portion of an IIR filter, so T_avg could potentially
       * wander very far during periods of noise/nonsense input.
       *
       * \param period
       * Maximum average clock period, T_avg_max, in units of input stream
       * sample clocks.
       */
      void set_max_avg_period(float period);

      /*!
       * \brief Set the minimum average clock period estimate limit, T_avg_min.
       *
       * \details
       * Sets the minimum average clock period estimate limit, T_avg_min
       * in units of input stream sample clocks (so the minimum average number
       * of input samples per output symbol, aka minimum samples/symbol).
       *
       * This limit is needed because T_avg is essentially computed by the
       * integrator portion of an IIR filter, so T_avg could potentially
       * wander very far during periods of noise/nonsense input.
       *
       * \param period
       * Minimum average clock period, T_avg_min, in units of input stream
       * sample clocks.
       */
      void set_min_avg_period(float period);

      /*!
       * \brief Set the nominal average clock period estimate limit, T_avg_nom.
       *
       * \details
       * Sets the nominal average clock period estimate limit, T_avg_nom
       * in units of input stream sample clocks (so the nominal average number
       * of input samples per output symbol, aka minimum samples/symbol).
       *
       * \param period
       * Nominal average clock period, T_avg_nom, in units of input stream
       * sample clocks.
       */
      void set_nom_avg_period(float period);

      /*******************************************************************
       * GET FUNCTIONS
       *******************************************************************/

      /*!
       * \brief Returns the normalized approximate loop bandwidth.
       *
       * \details
       * See the documentation for set_loop_bandwidth() for more details.
       *
       * Note that if set_alpha() or set_beta() were called to directly
       * set gains, the value returned by this method will be inaccurate/stale.
       */
      float get_loop_bandwidth() const;

      /*!
       * \brief Returns the loop damping factor.
       *
       * \details
       * See the documentation for set_damping_factor() for more details.
       *
       * Note that if set_alpha() or set_beta() were called to directly
       * set gains, the value returned by this method will be inaccurate/stale.
       */
      float get_damping_factor() const;

      /*!
       * \brief Returns the user providded expected gain of the Timing Error
       * Detector.
       *
       * \details
       * See the documentation for set_ted_gain() for more details.
       */
      float get_ted_gain() const;

      /*!
       * \brief Returns the PI filter proportional gain, alpha.
       *
       * \details
       * See the documentation for set_alpha() for more details.
       */
      float get_alpha() const;

      /*!
       * \brief Returns the PI filter integral gain, beta.
       *
       * \details
       * See the documentation for set_beta() for more details.
       */
      float get_beta() const;

      /*!
       * \brief Get the average clock period estimate, T_avg.
       *
       * \details
       * Gets the average clock period estimate, T_avg,
       * in units of input stream sample clocks (so the average number of
       * input samples per output symbol, aka samples/symbol).
       *
       * To convert to seconds, divide by the input stream sample rate:
       * F_s_input.
       */
      float get_avg_period() const;

      /*!
       * \brief Get the instantaneous clock period estimate, T_inst.
       *
       * \details
       * Gets the instantaneous clock period estimate, T_inst,
       * in units of input stream sample clocks (so the instantaneous number of
       * input samples per output symbol, aka instantaneous samples/symbol).
       *
       * To convert to seconds, divide by the input stream sample rate:
       * F_s_input.
       */
      float get_inst_period() const;

      /*!
       * \brief Get the instantaneous clock phase estimate, tau.
       *
       * \details
       * Gets the instantaneous clock phase estimate, tau, in units of
       * input stream sample clocks.
       *
       * To convert to seconds, divide by the input stream sample rate:
       * F_s_input.
       *
       * To manually wrap, add or subtract a multiple of the estimate of the 
       * average clock period, T_avg.
       *
       * To convert to a normalized (but not wrapped) clock phase estimate,
       * divide by the estimate of the average clock period, T_avg.  
       * To further convert the normalized clock phase estimate to radians,
       * multiply the normalized clock phase estimate by 2*pi.
       */
      float get_phase() const;

      /*!
       * \brief Get the maximum average clock period estimate limit, T_avg_max.
       *
       * \details
       * See the documentation for set_max_avg_period() for more details.
       */
      float get_max_avg_period() const;

      /*!
       * \brief Get the minimum average clock period estimate limit, T_avg_min.
       *
       * \details
       * See the documentation for set_min_avg_period() for more details.
       */
      float get_min_avg_period() const;

      /*!
       * \brief Get the nominal average clock period, T_avg_nom.
       *
       * \details
       * Gets the nominal average clock period, T_avg_nom,
       * in units of input stream sample clocks (so the nominal average
       * number of input samples per output symbol, aka nominal samples/symbol).
       *
       * To convert to seconds, divide by the input stream sample rate:
       * F_s_input.
       */
      float get_nom_avg_period() const;

    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CLOCK_TRACKING_LOOP_H */
