/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#ifndef INCLUDED_CVSD_ENCODER_SB_H
#define INCLUDED_CVSD_ENCODER_SB_H

#include <gr_sync_decimator.h>

class cvsd_encode_sb;

typedef boost::shared_ptr<cvsd_encode_sb> cvsd_encode_sb_sptr;

cvsd_encode_sb_sptr cvsd_make_encode_sb (short min_step=10,
					 short max_step=1280,
					 double step_decay=0.9990234375,
					 double accum_decay= 0.96875,
					 int K=32,
					 int J=4,
					 short pos_accum_max=32767,
					 short neg_accum_max=-32767);

class cvsd_encode_sb : public gr_sync_decimator
{
private:
  friend cvsd_encode_sb_sptr cvsd_make_encode_sb (short min_step,
						  short max_step,
						  double step_decay,
						  double accum_decay,
						  int K,
						  int J,
						  short pos_accum_max,
						  short neg_accum_max);

  cvsd_encode_sb (short min_step, short max_step, double step_decay,
		  double accum_decay, int K, int J,
		  short pos_accum_max, short neg_accum_max);
  
  int cvsd_round(double input);
  unsigned int cvsd_pow (short radix, short power);
  unsigned char cvsd_bitwise_sum (unsigned int input);

  short d_min_step;
  short d_max_step;
  double d_step_decay;
  double d_accum_decay;

  //! \brief Size of shift register; the number of output bits remembered in shift register
  int d_K;

  //! \brief Number of bits in the shift register that are equal; size of run of 1s, 0s
  int d_J;

  short d_pos_accum_max;
  short d_neg_accum_max;

  int d_accum;
  int d_loop_counter;
  unsigned int d_runner;
  short d_stepsize;

 public:
  ~cvsd_encode_sb ();	// public destructor

  short min_step() { return d_min_step; }
  short max_step() { return d_max_step; }
  double step_decay() { return d_step_decay; }
  double accum_decay() { return d_accum_decay; }
  int K() { return d_K; }
  int J() { return d_J; }
  short pos_accum_max() { return d_pos_accum_max; }
  short neg_accum_max() { return d_neg_accum_max; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CVSD_ENCODE_SB_H */
