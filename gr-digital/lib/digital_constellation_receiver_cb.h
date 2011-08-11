/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <gr_block.h>
#include <digital_constellation.h>
#include <gr_complex.h>
#include <math.h>
#include <fstream>

class digital_constellation_receiver_cb;
typedef boost::shared_ptr<digital_constellation_receiver_cb> digital_constellation_receiver_cb_sptr;

// public constructor
digital_constellation_receiver_cb_sptr 
digital_make_constellation_receiver_cb (digital_constellation_sptr constellation,
					float loop_bw, float fmin, float fmax);

/*!
 * \brief This block takes care of receiving generic modulated signals through phase, frequency, and symbol
 * synchronization. 
 * \ingroup sync_blk
 * \ingroup demod_blk
 *
 * This block takes care of receiving generic modulated signals through phase, frequency, and symbol
 * synchronization. It performs carrier frequency and phase locking as well as symbol timing recovery. 
 *
 * The phase and frequency synchronization are based on a Costas loop that finds the error of the incoming
 * signal point compared to its nearest constellation point. The frequency and phase of the NCO are 
 * updated according to this error.
 *
 * The symbol synchronization is done using a modified Mueller and Muller circuit from the paper:
 * 
 *    G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller and Muller 
 *    algorithm,"  Electronics Letters, Vol. 31, no. 13,  22 June 1995, pp. 1032 - 1033.
 *
 * This circuit interpolates the downconverted sample (using the NCO developed by the Costas loop)
 * every mu samples, then it finds the sampling error based on this and the past symbols and the decision
 * made on the samples. Like the phase error detector, there are optimized decision algorithms for BPSK
 * and QPKS, but 8PSK uses another brute force computation against all possible symbols. The modifications
 * to the M&M used here reduce self-noise.
 *
 */

class digital_constellation_receiver_cb : public gr_block
{
 public:
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);


  /*******************************************************************
    SET FUNCTIONS
  *******************************************************************/  

  /*!
   * \brief Set the loop bandwidth
   *
   * Set the loop filter's bandwidth to \p bw. This should be between 
   * 2*pi/200 and 2*pi/100  (in rads/samp). It must also be a positive
   * number.
   *
   * When a new damping factor is set, the gains, alpha and beta, of the loop
   * are recalculated by a call to update_gains().
   *
   * \param bw    (float) new bandwidth
   *
   */
  void set_loop_bandwidth(float bw);

  /*!
   * \brief Set the loop damping factor
   *
   * Set the loop filter's damping factor to \p df. The damping factor
   * should be sqrt(2)/2.0 for critically damped systems.
   * Set it to anything else only if you know what you are doing. It must
   * be a number between 0 and 1.
   *
   * When a new damping factor is set, the gains, alpha and beta, of the loop
   * are recalculated by a call to update_gains().
   *
   * \param df    (float) new damping factor
   *
   */
  void set_damping_factor(float df);

  /*!
   * \brief Set the loop gain alpha
   *
   * Set's the loop filter's alpha gain parameter.
   *
   * This value should really only be set by adjusting the loop bandwidth
   * and damping factor.
   *
   * \param alpha    (float) new alpha gain
   *
   */
  void set_alpha(float alpha);

  /*!
   * \brief Set the loop gain beta
   *
   * Set's the loop filter's beta gain parameter.
   *
   * This value should really only be set by adjusting the loop bandwidth
   * and damping factor.
   *
   * \param beta    (float) new beta gain
   *
   */
  void set_beta(float beta);

  /*!
   * \brief Set the phase/freq recovery loop's frequency.
   *
   * Set's the phase/freq recovery loop's frequency. While this is normally
   * updated by the inner loop of the algorithm, it could be useful to
   * manually initialize, set, or reset this under certain circumstances.
   *
   * \param freq    (float) new frequency
   *
   */
  void set_frequency(float freq);

  /*!
   * \brief Set the phase/freq recovery loop's phase.
   *
   * Set's the phase/freq recovery loop's phase. While this is normally
   * updated by the inner loop of the algorithm, it could be useful to
   * manually initialize, set, or reset this under certain circumstances.
   *
   * \param phase    (float) new phase
   *
   */
  void set_phase(float phase);

  /*******************************************************************
    GET FUNCTIONS
  *******************************************************************/

  /*!
   * \brief Returns the loop bandwidth
   */
  float get_loop_bandwidth() const;

  /*!
   * \brief Returns the loop damping factor
   */
  float get_damping_factor() const;

  /*!
   * \brief Returns the loop gain alpha
   */
  float get_alpha() const;

  /*!
   * \brief Returns the loop gain beta
   */
  float get_beta() const;

  /*!
   * \brief Get the phase/freq recovery loop's frequency estimate
   */
  float get_frequency() const;

  /*!
   * \brief Get the phase/freq loop's phase estimate
   */
  float get_phase() const;

protected:

 /*!
   * \brief Constructor to synchronize incoming M-PSK symbols
   *
   * \param constellation	constellation of points for generic modulation
   * \param alpha	gain parameter to adjust the phase in the Costas loop (~0.01)
   * \param beta        gain parameter to adjust the frequency in the Costas loop (~alpha^2/4)	
   * \param fmin        minimum normalized frequency value the loop can achieve
   * \param fmax        maximum normalized frequency value the loop can achieve
   *
   * The constructor also chooses which phase detector and decision maker to use in the
   * work loop based on the value of M.
   */
  digital_constellation_receiver_cb (digital_constellation_sptr constellation, 
				     float loop_bw, float fmin, float fmax);

  void phase_error_tracking(float phase_error);

  private:
  unsigned int d_M;

  // Members related to carrier and phase tracking
  float d_freq, d_max_freq, d_min_freq;
  float d_phase;

  float d_loop_bw;
  float d_damping;
  float d_alpha;
  float d_beta;

  digital_constellation_sptr d_constellation;
  unsigned int d_current_const_point;

  //! delay line length.
  static const unsigned int DLLEN = 8;
  
  //! delay line plus some length for overflow protection
  gr_complex d_dl[2*DLLEN] __attribute__ ((aligned(8)));
  
  //! index to delay line
  unsigned int d_dl_idx;

  /*! \brief update the system gains from the loop bandwidth and damping factor
   *
   *  This function updates the system gains based on the loop
   *  bandwidth and damping factor of the system.
   *  These two factors can be set separately through their own
   *  set functions.
   */
  void update_gains();

  friend digital_constellation_receiver_cb_sptr
  digital_make_constellation_receiver_cb (digital_constellation_sptr constell,
					  float loop_bw, float fmin, float fmax);
};

#endif
