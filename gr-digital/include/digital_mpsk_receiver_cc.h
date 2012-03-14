/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2011 Free Software Foundation, Inc.
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

#include <digital_api.h>
#include <gruel/attributes.h>
#include <gri_control_loop.h>
#include <gr_block.h>
#include <gr_complex.h>
#include <fstream>

class gri_mmse_fir_interpolator_cc;

class digital_mpsk_receiver_cc;
typedef boost::shared_ptr<digital_mpsk_receiver_cc> digital_mpsk_receiver_cc_sptr;

// public constructor
DIGITAL_API digital_mpsk_receiver_cc_sptr 
digital_make_mpsk_receiver_cc (unsigned int M, float theta, 
			       float loop_bw,
			       float fmin, float fmax,
			       float mu, float gain_mu, 
			       float omega, float gain_omega, float omega_rel);

/*!
 * \brief This block takes care of receiving M-PSK modulated signals
 * through phase, frequency, and symbol synchronization.
 * \ingroup sync_blk
 * \ingroup demod_blk
 * \ingroup digital
 *
 * This block takes care of receiving M-PSK modulated signals through
 * phase, frequency, and symbol synchronization. It performs carrier
 * frequency and phase locking as well as symbol timing recovery.  It
 * works with (D)BPSK, (D)QPSK, and (D)8PSK as tested currently. It
 * should also work for OQPSK and PI/4 DQPSK.
 *
 * The phase and frequency synchronization are based on a Costas loop
 * that finds the error of the incoming signal point compared to its
 * nearest constellation point. The frequency and phase of the NCO are
 * updated according to this error. There are optimized phase error
 * detectors for BPSK and QPSK, but 8PSK is done using a brute-force
 * computation of the constellation points to find the minimum.
 *
 * The symbol synchronization is done using a modified Mueller and
 * Muller circuit from the paper:
 * 
 *    G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller
 *    and Muller algorithm," Electronics Letters, Vol. 31, no. 13, 22
 *    June 1995, pp. 1032 - 1033.
 *
 * This circuit interpolates the downconverted sample (using the NCO
 * developed by the Costas loop) every mu samples, then it finds the
 * sampling error based on this and the past symbols and the decision
 * made on the samples. Like the phase error detector, there are
 * optimized decision algorithms for BPSK and QPKS, but 8PSK uses
 * another brute force computation against all possible symbols. The
 * modifications to the M&M used here reduce self-noise.
 *
 */

class DIGITAL_API digital_mpsk_receiver_cc : public gr_block, public gri_control_loop
{
 public:
  ~digital_mpsk_receiver_cc ();
  void forecast(int noutput_items, gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);


  //! Returns the modulation order (M) currently set
  float modulation_order() const { return d_M; }

  //! Returns current value of theta
  float theta() const { return d_theta; }

  //! Returns current value of mu
  float mu() const { return d_mu; }

  //! Returns current value of omega
  float omega() const { return d_omega; }

  //! Returns mu gain factor
  float gain_mu() const { return d_gain_mu; }

  //! Returns omega gain factor
  float gain_omega() const { return d_gain_omega; }

  //! Returns the relative omega limit
  float gain_omega_rel() const {return d_omega_rel; }

  //! Sets the modulation order (M) currently
  void set_modulation_order(unsigned int M);

  //! Sets value of theta
  void set_theta(float theta) { d_theta = theta; }

  //! Sets value of mu
  void set_mu (float mu) { d_mu = mu; }
  
  //! Sets value of omega and its min and max values 
  void set_omega (float omega) { 
    d_omega = omega;
    d_min_omega = omega*(1.0 - d_omega_rel);
    d_max_omega = omega*(1.0 + d_omega_rel);
    d_omega_mid = 0.5*(d_min_omega+d_max_omega);
  }

  //! Sets value for mu gain factor
  void set_gain_mu (float gain_mu) { d_gain_mu = gain_mu; }

  //! Sets value for omega gain factor
  void set_gain_omega (float gain_omega) { d_gain_omega = gain_omega; }

  //! Sets the relative omega limit and resets omega min/max values
  void set_gain_omega_rel(float omega_rel);
  
protected:
  
  /*!
   * \brief Constructor to synchronize incoming M-PSK symbols
   *
   * \param M	        modulation order of the M-PSK modulation
   * \param theta	any constant phase rotation from the real axis of the constellation
   * \param loop_bw	Loop bandwidth to set gains of phase/freq tracking loop
   * \param fmin        minimum normalized frequency value the loop can achieve
   * \param fmax        maximum normalized frequency value the loop can achieve
   * \param mu          initial parameter for the interpolator [0,1]
   * \param gain_mu     gain parameter of the M&M error signal to adjust mu (~0.05)
   * \param omega       initial value for the number of symbols between samples (~number of samples/symbol)
   * \param gain_omega  gain parameter to adjust omega based on the error (~omega^2/4)
   * \param omega_rel   sets the maximum (omega*(1+omega_rel)) and minimum (omega*(1+omega_rel)) omega (~0.005)
   *
   * The constructor also chooses which phase detector and decision maker to use in the work loop based on the
   * value of M.
   */
  digital_mpsk_receiver_cc (unsigned int M, float theta, 
			    float loop_bw,
			    float fmin, float fmax,
			    float mu, float gain_mu, 
			    float omega, float gain_omega, float omega_rel);

  void make_constellation();
  void mm_sampler(const gr_complex symbol);
  void mm_error_tracking(gr_complex sample);
  void phase_error_tracking(gr_complex sample);


  /*!
   * \brief Phase error detector for MPSK modulations.
   *
   * \param sample   the I&Q sample from which to determine the phase error
   *
   * This function determines the phase error for any MPSK signal by
   * creating a set of PSK constellation points and doing a
   * brute-force search to see which point minimizes the Euclidean
   * distance. This point is then used to derotate the sample to the
   * real-axis and a atan (using the fast approximation function) to
   * determine the phase difference between the incoming sample and
   * the real constellation point
   *
   * This should be cleaned up and made more efficient.
   *
   * \returns the approximated phase error.
   */
  float phase_error_detector_generic(gr_complex sample) const; // generic for M but more costly

  /*!
   * \brief Phase error detector for BPSK modulation.
   *
   * \param sample   the I&Q sample from which to determine the phase error
   *
   * This function determines the phase error using a simple BPSK
   * phase error detector by multiplying the real and imaginary (the
   * error signal) components together. As the imaginary part goes to
   * 0, so does this error.
   *
   * \returns the approximated phase error.
   */
  float phase_error_detector_bpsk(gr_complex sample) const;    // optimized for BPSK

  /*!
   * \brief Phase error detector for QPSK modulation.
   *
   * \param sample   the I&Q sample from which to determine the phase error
   *
   * This function determines the phase error using the limiter
   * approach in a standard 4th order Costas loop
   *
   * \returns the approximated phase error.
   */
  float phase_error_detector_qpsk(gr_complex sample) const;



  /*!
   * \brief Decision maker for a generic MPSK constellation.
   *
   * \param sample   the baseband I&Q sample from which to make the decision
   *
   * This decision maker is a generic implementation that does a
   * brute-force search for the constellation point that minimizes the
   * error between it and the incoming signal.
   *
   * \returns the index to d_constellation that minimizes the error/
 */
  unsigned int decision_generic(gr_complex sample) const;


 /*!
   * \brief Decision maker for BPSK constellation.
   *
   * \param sample   the baseband I&Q sample from which to make the decision
   *
   * This decision maker is a simple slicer function that makes a
   * decision on the symbol based on its placement on the real axis of
   * greater than 0 or less than 0; the quadrature component is always
   * 0.
   *
   * \returns the index to d_constellation that minimizes the error/
   */
  unsigned int decision_bpsk(gr_complex sample) const;
  

  /*!
   * \brief Decision maker for QPSK constellation.
   *
   * \param sample   the baseband I&Q sample from which to make the decision
   *
   * This decision maker is a simple slicer function that makes a
   * decision on the symbol based on its placement versus both axes
   * and returns which quadrant the symbol is in.
   *
   * \returns the index to d_constellation that minimizes the error/
   */
  unsigned int decision_qpsk(gr_complex sample) const;

private:
  unsigned int d_M;
  float        d_theta;

  /*!
   * \brief Decision maker function pointer 
   *
   * \param sample   the baseband I&Q sample from which to make the decision
   *
   * This is a function pointer that is set in the constructor to
   * point to the proper decision function for the specified
   * constellation order.
   *
   * \return index into d_constellation point that is the closest to the recieved sample
   */
  unsigned int (digital_mpsk_receiver_cc::*d_decision)(gr_complex sample) const; // pointer to decision function


  std::vector<gr_complex> d_constellation;
  unsigned int d_current_const_point;

  // Members related to symbol timing
  float d_mu, d_gain_mu;
  float d_omega, d_gain_omega, d_omega_rel, d_max_omega, d_min_omega, d_omega_mid;
  gr_complex d_p_2T, d_p_1T, d_p_0T;
  gr_complex d_c_2T, d_c_1T, d_c_0T;

  /*!
   * \brief Phase error detector function pointer 
   *
   * \param sample   the I&Q sample from which to determine the phase error
   *
   * This is a function pointer that is set in the constructor to
   * point to the proper phase error detector function for the
   * specified constellation order.
   */
  float (digital_mpsk_receiver_cc::*d_phase_error_detector)(gr_complex sample) const;


  //! get interpolated value
  gri_mmse_fir_interpolator_cc 	*d_interp;
  
  //! delay line length.
  static const unsigned int DLLEN = 8;
  
  //! delay line plus some length for overflow protection
  __GR_ATTR_ALIGNED(8) gr_complex d_dl[2*DLLEN];
  
  //! index to delay line
  unsigned int d_dl_idx;

  friend DIGITAL_API digital_mpsk_receiver_cc_sptr
  digital_make_mpsk_receiver_cc (unsigned int M, float theta,
				 float loop_bw,
				 float fmin, float fmax,
				 float mu, float gain_mu, 
				 float omega, float gain_omega, float omega_rel);
};

#endif
