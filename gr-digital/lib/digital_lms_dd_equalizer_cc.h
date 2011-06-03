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

#ifndef INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_H
#define INCLUDED_DIGITAL_LMS_DD_EQUALIZER_CC_H

#include <gr_sync_block.h>
#include <digital_constellation.h>

class digital_lms_dd_equalizer_cc;
typedef boost::shared_ptr<digital_lms_dd_equalizer_cc> digital_lms_dd_equalizer_cc_sptr;

digital_lms_dd_equalizer_cc_sptr digital_make_lms_dd_equalizer_cc (float mu, int ntaps,
								   digital_constellation_sptr cnst);

/*!
 * \brief Least-Mean-Square Decision Directed Equalizer (complex in/out)
 * \ingroup eq_blk
 *
 * This block implements an LMS-based decision-directed equalizer.
 * It uses a set of weights, w, to correlate against the inputs, u,
 * and a decisions is then made from this output. The error
 * in the decision is used to update teh weight vector.
 *
 * y[n] = conj(w[n]) u[n]
 * d[n] = decision(y[n])
 * e[n] = d[n] - y[n]
 * w[n+1] = w[n] + mu u[n] conj(e[n])
 *
 * Where mu is a gain value (between 0 and 1 and usualy small,
 * around 0.01 - 0.1.
 *
 * This block uses the digital_constellation object for making
 * the decision from y[n]. Create the constellation object for
 * whatever constellation is to be used and pass in the object.
 * In Python, you can use something like:
 *    self.constellation = digital.constellation_qpsk()
 * To create a QPSK constellation (see the digital_constellation
 * block for more details as to what constellations are available
 * or how to create your own). You then pass the object to this
 * block as an sptr, or using "self.constellation.base()".
 *
 * The theory for this algorithm can be found in Chapter 9 of:
 * S. Haykin, Adaptive Filter Theory, Upper Saddle River, NJ:
 *    Prentice Hall, 1996.
 *
 */
class digital_lms_dd_equalizer_cc : public gr_sync_block
{
private:
  friend digital_lms_dd_equalizer_cc_sptr digital_make_lms_dd_equalizer_cc (float mu, int ntaps,
									    digital_constellation_sptr cnst);
  
  float	d_mu;
  std::vector<gr_complex>  d_taps;
  digital_constellation_sptr d_cnst;

  digital_lms_dd_equalizer_cc (float mu, int ntaps,
			       digital_constellation_sptr cnst);

public:
  float get_mu();
  void  set_mu(float mu);
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
