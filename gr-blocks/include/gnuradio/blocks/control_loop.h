/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef GR_BLOCKS_CONTROL_LOOP
#define GR_BLOCKS_CONTROL_LOOP

#include <gnuradio/blocks/api.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief A second-order control loop implementation class.
     *
     * \details
     * This class implements a second order control loop and is
     * inteded to act as a parent class to blocks which need a control
     * loop (e.g., gr::digital::costas_loop_cc,
     * gr::analog::pll_refout_cc, etc.). It takes in a loop bandwidth
     * as well as a max and min frequency and provides the functions
     * that control the update of the loop.
     *
     * The loop works of alpha and beta gains. These gains are
     * calculated using the input loop bandwidth and a pre-set damping
     * factor. The damping factor can be changed using the
     * #set_damping_factor after the block is
     * constructed. The alpha and beta values can be set using their
     * respective #set_alpha or #set_beta functions if very precise
     * control over these is required.
     *
     * The class tracks both phase and frequency of a signal based on
     * an error signal. The error calculation is unique for each
     * algorithm and is calculated externally and passed to the
     * advance_loop function, which uses this to update its phase and
     * frequency estimates.
     *
     * This class also provides the functions #phase_wrap and
     * #frequency_limit to easily keep the phase and frequency
     * estimates within our set bounds (phase_wrap keeps it within
     * +/-2pi).
     */
    class BLOCKS_API control_loop
    {
    protected:
      float d_phase, d_freq;
      float d_max_freq, d_min_freq;
      float d_damping, d_loop_bw;
      float d_alpha, d_beta;

    public:
      control_loop(void) {}
      control_loop(float loop_bw, float max_freq, float min_freq);
      virtual ~control_loop();

      /*! \brief Update the system gains from the loop bandwidth and damping factor.
       *
       * \details
       * This function updates the system gains based on the loop
       * bandwidth and damping factor of the system. These two
       * factors can be set separately through their own set
       * functions.
       */
      void update_gains();

      /*! \brief Advance the control loop based on the current gain
       *  settings and the inputted error signal.
       */
      void advance_loop(float error);

      /*! \brief Keep the phase between -2pi and 2pi.
       *
       * \details
       * This function keeps the phase between -2pi and 2pi. If the
       * phase is greater than 2pi by d, it wraps around to be -2pi+d;
       * similarly if it is less than -2pi by d, it wraps around to
       * 2pi-d.
       *
       * This function should be called after advance_loop to keep the
       * phase in a good operating region. It is set as a separate
       * method in case another way is desired as this is fairly
       * heavy-handed.
       */
      void phase_wrap();

      /*! \brief Keep the frequency between d_min_freq and d_max_freq.
       *
       * \details
       * This function keeps the frequency between d_min_freq and
       * d_max_freq. If the frequency is greater than d_max_freq, it
       * is set to d_max_freq.  If the frequency is less than
       * d_min_freq, it is set to d_min_freq.
       *
       * This function should be called after advance_loop to keep the
       * frequency in the specified region. It is set as a separate
       * method in case another way is desired as this is fairly
       * heavy-handed.
       */
      void frequency_limit();

      /*******************************************************************
       * SET FUNCTIONS
       *******************************************************************/

      /*!
       * \brief Set the loop bandwidth.
       *
       * \details
       * Set the loop filter's bandwidth to \p bw. This should be
       * between 2*pi/200 and 2*pi/100 (in rads/samp). It must also be
       * a positive number.
       *
       * When a new damping factor is set, the gains, alpha and beta,
       * of the loop are recalculated by a call to update_gains().
       *
       * \param bw    (float) new bandwidth
       */
      virtual void set_loop_bandwidth(float bw);

      /*!
       * \brief Set the loop damping factor.
       *
       * \details
       * Set the loop filter's damping factor to \p df. The damping
       * factor should be sqrt(2)/2.0 for critically damped systems.
       * Set it to anything else only if you know what you are
       * doing. It must be a number between 0 and 1.
       *
       * When a new damping factor is set, the gains, alpha and beta,
       * of the loop are recalculated by a call to update_gains().
       *
       * \param df    (float) new damping factor
       */
      void set_damping_factor(float df);

      /*!
       * \brief Set the loop gain alpha.
       *
       * \details
       * Sets the loop filter's alpha gain parameter.
       *
       * This value should really only be set by adjusting the loop
       * bandwidth and damping factor.
       *
       * \param alpha    (float) new alpha gain
       */
      void set_alpha(float alpha);

      /*!
       * \brief Set the loop gain beta.
       *
       * \details
       * Sets the loop filter's beta gain parameter.
       *
       * This value should really only be set by adjusting the loop
       * bandwidth and damping factor.
       *
       * \param beta    (float) new beta gain
       */
      void set_beta(float beta);

      /*!
       * \brief Set the control loop's frequency.
       *
       * \details
       * Sets the control loop's frequency. While this is normally
       * updated by the inner loop of the algorithm, it could be
       * useful to manually initialize, set, or reset this under
       * certain circumstances.
       *
       * \param freq    (float) new frequency
       */
      void set_frequency(float freq);

      /*!
       * \brief Set the control loop's phase.
       *
       * \details
       * Sets the control loop's phase. While this is normally
       * updated by the inner loop of the algorithm, it could be
       * useful to manually initialize, set, or reset this under
       * certain circumstances.
       *
       * \param phase    (float) new phase
       */
      void set_phase(float phase);

      /*!
       * \brief Set the control loop's maximum frequency.
       *
       * \details
       * Set the maximum frequency the control loop can track. 
       *
       * \param freq    (float) new max frequency
       */
      void set_max_freq(float freq);

      /*!
       * \brief Set the control loop's minimum frequency.
       *
       * \details
       * Set the minimum frequency the control loop can track. 
       *
       * \param freq    (float) new min frequency
       */
      void set_min_freq(float freq);

      /*******************************************************************
       * GET FUNCTIONS
       *******************************************************************/

      /*!
       * \brief Returns the loop bandwidth.
       */
      float get_loop_bandwidth() const;

      /*!
       * \brief Returns the loop damping factor.
       */
      float get_damping_factor() const;

      /*!
       * \brief Returns the loop gain alpha.
       */
      float get_alpha() const;

      /*!
       * \brief Returns the loop gain beta.
       */
      float get_beta() const;

      /*!
       * \brief Get the control loop's frequency estimate.
       */
      float get_frequency() const;

      /*!
       * \brief Get the control loop's phase estimate.
       */
      float get_phase() const;

      /*!
       * \brief Get the control loop's maximum frequency.
       */
      float get_max_freq() const;

      /*!
       * \brief Get the control loop's minimum frequency.
       */
      float get_min_freq() const;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* GR_BLOCKS_CONTROL_LOOP */
