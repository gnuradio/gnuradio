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

#include <gr_digital_api.h>
#include <gruel/attributes.h>
#include <gr_block.h>
#include <digital_constellation.h>
#include <gr_complex.h>
#include <math.h>
#include <fstream>

class digital_constellation_receiver_cb;
typedef boost::shared_ptr<digital_constellation_receiver_cb> digital_constellation_receiver_cb_sptr;

// public constructor
GR_DIGITAL_API digital_constellation_receiver_cb_sptr 
digital_make_constellation_receiver_cb (digital_constellation_sptr constellation,
					float alpha, float beta,
					float fmin, float fmax);

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

class GR_DIGITAL_API digital_constellation_receiver_cb : public gr_block
{
 public:
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);


  // Member function related to the phase/frequency tracking portion of the receiver
  //! (CL) Returns the value for alpha (the phase gain term)
  float alpha() const { return d_alpha; }
  
  //! (CL) Returns the value of beta (the frequency gain term)
  float beta() const { return d_beta; }

  //! (CL) Returns the current value of the frequency of the NCO in the Costas loop
  float freq() const { return d_freq; }

  //! (CL) Returns the current value of the phase of the NCO in the Costal loop
  float phase() const { return d_phase; }

  //! (CL) Sets the value for alpha (the phase gain term)
  void set_alpha(float alpha) { d_alpha = alpha; }
  
  //! (CL) Setss the value of beta (the frequency gain term)
  void set_beta(float beta) { d_beta = beta; }

  //! (CL) Sets the current value of the frequency of the NCO in the Costas loop
  void set_freq(float freq) { d_freq = freq; }

  //! (CL) Setss the current value of the phase of the NCO in the Costal loop
  void set_phase(float phase) { d_phase = phase; }


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
				     float alpha, float beta,
				     float fmin, float fmax);

  void phase_error_tracking(float phase_error);

  private:
  unsigned int d_M;

  // Members related to carrier and phase tracking
  float d_alpha;
  float d_beta;
  float d_freq, d_max_freq, d_min_freq;
  float d_phase;

  digital_constellation_sptr d_constellation;
  unsigned int d_current_const_point;

  //! delay line length.
  static const unsigned int DLLEN = 8;
  
  //! delay line plus some length for overflow protection
  __GR_ATTR_ALIGNED(8) gr_complex d_dl[2*DLLEN];
  
  //! index to delay line
  unsigned int d_dl_idx;

  friend GR_DIGITAL_API digital_constellation_receiver_cb_sptr
  digital_make_constellation_receiver_cb (digital_constellation_sptr constell,
					  float alpha, float beta,
					  float fmin, float fmax);
};

#endif
