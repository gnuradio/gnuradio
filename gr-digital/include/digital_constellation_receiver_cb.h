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

#include <digital_api.h>
#include <gr_block.h>
#include <digital_constellation.h>
#include <gruel/attributes.h>
#include <gri_control_loop.h>
#include <gr_complex.h>
#include <math.h>
#include <fstream>

class digital_constellation_receiver_cb;
typedef boost::shared_ptr<digital_constellation_receiver_cb> digital_constellation_receiver_cb_sptr;

// public constructor
DIGITAL_API digital_constellation_receiver_cb_sptr 
digital_make_constellation_receiver_cb (digital_constellation_sptr constellation,
					float loop_bw, float fmin, float fmax);

/*!
 * \brief This block does fine-phase and frequency locking and decision making.
 * \ingroup symbol_coding_blk
 * \ingroup modulators_blk
 *
 * \details
 * The phase and frequency synchronization are based on a Costas loop
 * that finds the error of the incoming signal point compared to its
 * nearest constellation point. The frequency and phase of the NCO are
 * updated according to this error.
 *
 * The decicision making itself is performed by the appropriate method of the
 * passed constellation object.
 *
 */

class DIGITAL_API digital_constellation_receiver_cb : public gr_block, public gri_control_loop
{
public:
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

protected:

 /*!
   * \brief Constructor to synchronize incoming M-PSK symbols
   *
   * \param constellation constellation object for generic demodulation
   * \param loop_bw	Loop bandwidth of the Costas Loop (~ 2pi/100)
   * \param fmin        minimum normalized frequency value the loop can achieve
   * \param fmax        maximum normalized frequency value the loop can achieve
   *
   */
  digital_constellation_receiver_cb (digital_constellation_sptr constellation, 
				     float loop_bw, float fmin, float fmax);

  void phase_error_tracking(float phase_error);

private:
  unsigned int d_M;

  digital_constellation_sptr d_constellation;
  unsigned int d_current_const_point;

  //! delay line length.
  static const unsigned int DLLEN = 8;
  
  //! delay line plus some length for overflow protection
  __GR_ATTR_ALIGNED(8) gr_complex d_dl[2*DLLEN];
  
  //! index to delay line
  unsigned int d_dl_idx;

  friend DIGITAL_API digital_constellation_receiver_cb_sptr
  digital_make_constellation_receiver_cb (digital_constellation_sptr constell,
					  float loop_bw, float fmin, float fmax);
};

#endif
