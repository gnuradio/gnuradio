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

#ifndef INCLUDED_GR_BIN_STATISTICS_F_H
#define INCLUDED_GR_BIN_STATISTICS_F_H


#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_feval.h>
#include <gr_message.h>
#include <gr_msg_queue.h>

class gr_bin_statistics_f;
typedef boost::shared_ptr<gr_bin_statistics_f> gr_bin_statistics_f_sptr;


GR_CORE_API gr_bin_statistics_f_sptr
gr_make_bin_statistics_f(unsigned int vlen,	// vector length
			 gr_msg_queue_sptr msgq,
			 gr_feval_dd *tune,	// callback
			 size_t tune_delay,	// samples
			 size_t dwell_delay);	// samples
			
/*!
 * \brief control scanning and record frequency domain statistics
 * \ingroup sink_blk
 */
class GR_CORE_API gr_bin_statistics_f : public gr_sync_block
{
  friend GR_CORE_API gr_bin_statistics_f_sptr
  gr_make_bin_statistics_f(unsigned int vlen,	   // vector length
			   gr_msg_queue_sptr msgq,
			   gr_feval_dd *tune,      // callback
			   size_t tune_delay,	   // samples
			   size_t dwell_delay);	   // samples

  enum state_t { ST_INIT, ST_TUNE_DELAY, ST_DWELL_DELAY };

  size_t	     d_vlen;
  gr_msg_queue_sptr  d_msgq;
  gr_feval_dd       *d_tune;
  size_t	     d_tune_delay;
  size_t	     d_dwell_delay;
  double	     d_center_freq;

  state_t	     d_state;
  size_t	     d_delay;	// nsamples remaining to state transition

  gr_bin_statistics_f(unsigned int vlen,
		      gr_msg_queue_sptr msgq,
		      gr_feval_dd *tune,
		      size_t tune_delay,
		      size_t dwell_delay);

  void enter_init();
  void enter_tune_delay();
  void enter_dwell_delay();
  void leave_dwell_delay();

protected:
  std::vector<float> d_max;	// per bin maxima

  size_t vlen() const { return d_vlen; }
  double center_freq() const { return d_center_freq; }
  gr_msg_queue_sptr msgq() const { return d_msgq; }

  virtual void reset_stats();
  virtual void accrue_stats(const float *input);
  virtual void send_stats();

public:
  ~gr_bin_statistics_f();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
  
};

#endif
