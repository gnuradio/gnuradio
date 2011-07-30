/* -*- c++ -*- */
/*
 * Copyright 2006,2011 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_COSTAS_LOOP_CC_H
#define INCLUDED_DIGITAL_COSTAS_LOOP_CC_H

#include <gr_sync_block.h>
#include <stdexcept>
#include <fstream>


/*! \brief A Costas loop carrier recovery module.
 * \ingroup sync_blk
 *  
 *  The Costas loop locks to the center frequency of a signal and
 *  downconverts it to baseband. The second (order=2) order loop is
 *  used for BPSK where the real part of the output signal is the
 *  baseband BPSK signal and the imaginary part is the error
 *  signal. When order=4, it can be used for quadrature modulations
 *  where both I and Q (real and imaginary) are outputted.
 *
 *  More details can be found online:
 *
 *  J. Feigin, "Practical Costas loop design: Designing a simple and inexpensive
 *  BPSK Costas loop carrier recovery circuit," RF signal processing, pp. 20-36,
 *  2002.
 *
 *  http://rfdesign.com/images/archive/0102Feigin20.pdf
 *  
 * \param alpha the loop gain used for phase adjustment
 * \param beta the loop gain for frequency adjustments
 * \param max_freq the maximum frequency deviation (radians/sample) the loop can handle
 * \param min_freq the minimum frequency deviation (radians/sample) the loop can handle
 * \param order the loop order, either 2 or 4
 */
class digital_costas_loop_cc;
typedef boost::shared_ptr<digital_costas_loop_cc> digital_costas_loop_cc_sptr;


digital_costas_loop_cc_sptr 
digital_make_costas_loop_cc (float loop_bw, int order
			     ) throw (std::invalid_argument);


/*!
 * \brief Carrier tracking PLL for QPSK
 * \ingroup sync_blk
 * input: complex; output: complex
 * <br>The Costas loop can have two output streams:
 *    stream 1 is the baseband I and Q;
 *    stream 2 is the normalized frequency of the loop
 *
 * \p order must be 2 or 4.
 */
class digital_costas_loop_cc : public gr_sync_block
{
  friend digital_costas_loop_cc_sptr
  digital_make_costas_loop_cc (float loop_bw, int order
			       ) throw (std::invalid_argument);

  float d_max_freq;
  float d_min_freq;
  int d_order;

  float d_loop_bw;
  float d_damping;
  float d_alpha;
  float d_beta;

  float d_phase;
  float d_freq;

  digital_costas_loop_cc (float loop_bw, int order
			  ) throw (std::invalid_argument);
  
  /*! \brief update the system gains from omega and eta
   *
   *  This function updates the system gains based on the loop
   *  bandwidth and damping factor of the system.
   *  These two factors can be set separately through their own
   *  set functions.
   */
  void update_gains();

  /*! \brief the phase detector circuit for 8th-order PSK loops
   *  \param sample complex sample
   *  \return the phase error
   */
  float phase_detector_8(gr_complex sample) const;    // for 8PSK

  /*! \brief the phase detector circuit for fourth-order loops
   *  \param sample complex sample
   *  \return the phase error
   */
  float phase_detector_4(gr_complex sample) const;    // for QPSK

  /*! \brief the phase detector circuit for second-order loops
   *  \param sample a complex sample
   *  \return the phase error
   */
  float phase_detector_2(gr_complex sample) const;    // for BPSK


  float (digital_costas_loop_cc::*d_phase_detector)(gr_complex sample) const;

public:

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
   * \brief Set the Costas loop's frequency.
   *
   * Set's the Costas Loop's frequency. While this is normally updated by the
   * inner loop of the algorithm, it could be useful to manually initialize,
   * set, or reset this under certain circumstances.
   *
   * \param freq    (float) new frequency
   *
   */
  void set_frequency(float freq);

  /*!
   * \brief Set the Costas loop's phase.
   *
   * Set's the Costas Loop's phase. While this is normally updated by the
   * inner loop of the algorithm, it could be useful to manually initialize,
   * set, or reset this under certain circumstances.
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
   * \brief Get the Costas loop's frequency estimate
   */
  float get_frequency() const;

  /*!
   * \brief Get the Costas loop's phase estimate
   */
  float get_phase() const;

  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  /*! \brief returns the current NCO frequency in radians/sample
   *
   */
  float freq() const { return d_freq; }
};

#endif
