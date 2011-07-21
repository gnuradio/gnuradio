/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_CORRELATOR_H
#define INCLUDED_GR_SIMPLE_CORRELATOR_H

#include <gr_core_api.h>
#include <gr_block.h>
#include <assert.h>

//#define	DEBUG_SIMPLE_CORRELATOR

class gr_simple_correlator;
typedef boost::shared_ptr<gr_simple_correlator> gr_simple_correlator_sptr;

GR_CORE_API gr_simple_correlator_sptr gr_make_simple_correlator (int payload_bytesize);

/*!
 * \brief inverse of gr_simple_framer (more or less)
 * \ingroup sync_blk
 */
class GR_CORE_API gr_simple_correlator : public gr_block
{
  static const int OVERSAMPLE = 8;
  enum state_t { ST_LOOKING, ST_UNDER_THRESHOLD, ST_LOCKED };
  
  int	  	 d_payload_bytesize;
  state_t	 d_state;
  unsigned int	 d_osi;				// over sample index [0,OVERSAMPLE-1]
  unsigned int	 d_transition_osi;		// first index where Hamming dist < thresh
  unsigned int	 d_center_osi;			// center of bit
  unsigned long long int d_shift_reg[OVERSAMPLE];
  int		 d_bblen;			// length of bitbuf
  unsigned char	*d_bitbuf;			// demodulated bits
  unsigned char	*d_pktbuf;			// temp packet buf
  int		 d_bbi;				// bitbuf index

  static const int AVG_PERIOD = 512;		// must be power of 2 (for freq offset correction)
  int	d_avbi;
  float	d_avgbuf[AVG_PERIOD];
  float d_avg;
  float d_accum;

#ifdef DEBUG_SIMPLE_CORRELATOR
  FILE		*d_debug_fp;			// binary log file
#endif

  friend GR_CORE_API gr_simple_correlator_sptr gr_make_simple_correlator (int payload_bytesize);
  gr_simple_correlator (int payload_bytesize);


  inline int slice (float x)
  {
    return x >= d_avg ? 1 : 0;
  }

  void update_avg(float x);

  void enter_locked ();
  void enter_under_threshold ();
  void enter_looking ();
  
  static int add_index (int a, int b)
  {
    int t = a + b;
    if (t >= OVERSAMPLE)
      t -= OVERSAMPLE;
    assert (t >= 0 && t < OVERSAMPLE);
    return t;
  }

  static int sub_index (int a, int b)
  {
    int t = a - b;
    if (t < 0)
      t += OVERSAMPLE;
    assert (t >= 0 && t < OVERSAMPLE);
    return t;
  }

  
 public:
  ~gr_simple_correlator ();

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_SIMPLE_CORRELATOR_H */
