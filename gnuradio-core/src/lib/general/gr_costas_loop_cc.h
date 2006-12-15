/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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


#ifndef INCLUDED_GR_COSTAS_LOOP_CC_H
#define INCLUDED_GR_COSTAS_LOOP_CC_H

#include <gr_sync_block.h>
#include <stdexcept>
#include <fstream>

class gr_costas_loop_cc;
typedef boost::shared_ptr<gr_costas_loop_cc> gr_costas_loop_cc_sptr;

gr_costas_loop_cc_sptr 
gr_make_costas_loop_cc (float alpha, float beta,
			float max_freq, float min_freq, 
			int order
			) throw (std::invalid_argument);


/*!
 * \brief Carrier tracking PLL for QPSK
 * input: complex; output: complex
 * <br>The Costas loop can have two output streams:
 *    stream 1 is the baseband I&Q;
 *    stream 2 is the normalized frequency of the loop
 *
 * \p order must be 2 or 4.
 */
class gr_costas_loop_cc : public gr_sync_block
{
  friend gr_costas_loop_cc_sptr gr_make_costas_loop_cc (float alpha, float beta,
							float max_freq, float min_freq, 
							int order
							) throw (std::invalid_argument);

  float d_alpha, d_beta, d_max_freq, d_min_freq, d_phase, d_freq;
  int d_order;

  gr_costas_loop_cc (float alpha, float beta,
		     float max_freq, float min_freq, 
		     int order
		     ) throw (std::invalid_argument);

  float phase_detector_4(gr_complex sample) const;    // for QPSK
  float phase_detector_2(gr_complex sample) const;    // for BPSK
  float (gr_costas_loop_cc::*d_phase_detector)(gr_complex sample) const;

public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
