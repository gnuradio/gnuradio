/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_LMS_DFE_FF_H
#define INCLUDED_GR_LMS_DFE_FF_H

#include <gr_sync_block.h>

class gr_lms_dfe_ff;
typedef boost::shared_ptr<gr_lms_dfe_ff> gr_lms_dfe_ff_sptr;

gr_lms_dfe_ff_sptr gr_make_lms_dfe_ff (float lambda_ff, float lambda_fb,
				       unsigned int num_fftaps, unsigned int num_fbtaps);

/*!
 * \brief Least-Mean-Square Decision Feedback Equalizer (float in/out)
 * \ingroup misc
 */
class gr_lms_dfe_ff : public gr_sync_block
{
  friend gr_lms_dfe_ff_sptr gr_make_lms_dfe_ff (float lambda_ff, float lambda_fb,
						unsigned int num_fftaps, unsigned int num_fbtaps);
  
  float	d_lambda_ff;
  float	d_lambda_fb;
  std::vector<float>  d_ff_delayline;
  std::vector<float>  d_fb_delayline;
  std::vector<float>  d_ff_taps;
  std::vector<float>  d_fb_taps;
  unsigned int d_ff_index;
  unsigned int d_fb_index;

  gr_lms_dfe_ff (float lambda_ff, float lambda_fb, 
		 unsigned int num_fftaps, unsigned int num_fbtaps);
  
  public:
  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
