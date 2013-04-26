/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_AGC2_CC_H
#define INCLUDED_GR_AGC2_CC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_agc2_cc.h>

class gr_agc2_cc;
typedef boost::shared_ptr<gr_agc2_cc> gr_agc2_cc_sptr;

GR_CORE_API gr_agc2_cc_sptr
gr_make_agc2_cc (float attack_rate = 1e-1, float decay_rate = 1e-2, float reference = 1.0,
		 float gain = 1.0, float max_gain = 0.0);
/*!
 * \brief high performance Automatic Gain Control class
 *
 * For Power the absolute value of the complex number is used.
 */

class GR_CORE_API gr_agc2_cc : public gr_sync_block, public gri_agc2_cc
{
  friend GR_CORE_API gr_agc2_cc_sptr gr_make_agc2_cc (float attack_rate, float decay_rate, float reference,
					  float gain, float max_gain);
  gr_agc2_cc (float attack_rate, float decay_rate, float reference,
	      float gain, float max_gain);

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_AGC2_CC_H */
