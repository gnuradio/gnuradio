/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SINGLE_POLE_IIR_FILTER_FF_H
#define	INCLUDED_GR_SINGLE_POLE_IIR_FILTER_FF_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_single_pole_iir.h>
#include <stdexcept>

class gr_single_pole_iir_filter_ff;
typedef boost::shared_ptr<gr_single_pole_iir_filter_ff> gr_single_pole_iir_filter_ff_sptr;

GR_CORE_API gr_single_pole_iir_filter_ff_sptr 
gr_make_single_pole_iir_filter_ff (double alpha, unsigned int vlen=1);

/*!
 * \brief  single pole IIR filter with float input, float output
 * \ingroup filter_blk
 *
 * The input and output satisfy a difference equation of the form
 \htmlonly
 \f{
 y[n] - (1-alpha) y[n-1] = alpha x[n]
 \f}
 \endhtmlonly

 \xmlonly
 y[n] - (1-alpha) y[n-1] = alpha x[n]
 \endxmlonly

 * with the corresponding rational system function
 \htmlonly
 \f{
 H(z) = \frac{alpha}{1 - (1-alpha) z^{-1}}
 \f}
 \endhtmlonly

 \xmlonly
H(z) = \ frac{alpha}{1 - (1-alpha) z^{-1}}
 \endxmlonly

 * Note that some texts define the system function with a + in the denominator.
 * If you're using that convention, you'll need to negate the feedback tap.
 */
class GR_CORE_API gr_single_pole_iir_filter_ff : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_single_pole_iir_filter_ff_sptr 
  gr_make_single_pole_iir_filter_ff (double alpha, unsigned int vlen);

  unsigned int			        		d_vlen;
  std::vector<gr_single_pole_iir<float,float,double> >	d_iir;

  gr_single_pole_iir_filter_ff (double alpha, unsigned int vlen);

 public:
  ~gr_single_pole_iir_filter_ff ();

  void set_taps (double alpha);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
