/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FRACTIONAL_INTERPOLATOR_FF_H
#define	INCLUDED_GR_FRACTIONAL_INTERPOLATOR_FF_H

#include <gr_core_api.h>
#include <gr_block.h>

class gri_mmse_fir_interpolator;

class gr_fractional_interpolator_ff;
typedef boost::shared_ptr<gr_fractional_interpolator_ff> gr_fractional_interpolator_ff_sptr;

// public constructor
GR_CORE_API gr_fractional_interpolator_ff_sptr gr_make_fractional_interpolator_ff (float phase_shift, float interp_ratio);

/*!
 * \brief Interpolating mmse filter with float input, float output
 * \ingroup filter_blk
 */
class GR_CORE_API gr_fractional_interpolator_ff : public gr_block
{
public:
  ~gr_fractional_interpolator_ff ();
  void forecast(int noutput_items, gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

  float mu() const { return d_mu;}
  float interp_ratio() const { return d_mu_inc;}
  void set_mu (float mu) { d_mu = mu; }
  void set_interp_ratio (float interp_ratio) { d_mu_inc = interp_ratio; }

protected:
  gr_fractional_interpolator_ff (float phase_shift, float interp_ratio);

private:
  float 			d_mu;
  float 			d_mu_inc;
  gri_mmse_fir_interpolator 	*d_interp;

  friend GR_CORE_API gr_fractional_interpolator_ff_sptr
  gr_make_fractional_interpolator_ff (float phase_shift, float interp_ratio);
};

#endif
