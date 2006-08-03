/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_GR_SINGLE_POLE_REC_FILTER_FF_H
#define	INCLUDED_GR_SINGLE_POLE_REC_FILTER_FF_H

#include <gr_sync_block.h>
#include <gr_single_pole_avg.h>
#include <stdexcept>

class gr_single_pole_avg_filter_ff;
typedef boost::shared_ptr<gr_single_pole_avg_filter_ff> gr_single_pole_avg_filter_ff_sptr;

gr_single_pole_avg_filter_ff_sptr 
gr_make_single_pole_avg_filter_ff (double alpha, unsigned int vlen=1);

/*!
 * \brief  single pole moving average filter with float input, float output
 * \ingroup filter
 *
 * The input and output satisfy a difference equation of the form

 \f[
 y[n] - (1-alpha) y[n-1] = alpha x[n]
 \f]

 * with the corresponding rational system function

 \f[
 H(z) = \frac{alpha}{1 - (1-alpha) z^{-1}}
 \f]

 * Note that some texts define the system function with a + in the denominator.
 * If you're using that convention, you'll need to negate the feedback tap.
 */
class gr_single_pole_avg_filter_ff : public gr_sync_block
{
 private:
  friend gr_single_pole_avg_filter_ff_sptr 
  gr_make_single_pole_avg_filter_ff (double alpha, unsigned int vlen);

  unsigned int			        		d_vlen;
  std::vector<gr_single_pole_avg<float,float,double> >	d_rec;

  gr_single_pole_avg_filter_ff (double alpha, unsigned int vlen);

 public:
  ~gr_single_pole_avg_filter_ff ();

  void set_taps (double alpha);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
