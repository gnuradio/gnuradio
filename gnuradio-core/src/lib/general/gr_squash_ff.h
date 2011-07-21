/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef   	INCLUDED_GR_SQUASH_FF_H_
# define   	INCLUDED_GR_SQUASH_FF_H_

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
/*!
 * \brief implements cheap resampling of spectrum directly from
 * spectral points, using gsl interpolation
 * \ingroup misc
 */

class gr_squash_ff;
typedef boost::shared_ptr<gr_squash_ff> gr_squash_ff_sptr;

GR_CORE_API gr_squash_ff_sptr gr_make_squash_ff(const std::vector<float> &igrid,
				    const std::vector<float> &ogrid);

class GR_CORE_API gr_squash_ff : public gr_sync_block
{
  friend GR_CORE_API gr_squash_ff_sptr gr_make_squash_ff(const std::vector<float> &igrid,
					     const std::vector<float> &ogrid);
  
  size_t  d_inum;
  size_t  d_onum;
  double *d_igrid;
  double *d_iwork;
  double *d_ogrid;

  gsl_interp_accel *d_accel;
  gsl_spline       *d_spline;
  
  gr_squash_ff(const std::vector<float> &igrid,
	       const std::vector<float> &ogrid);

 public:
  ~gr_squash_ff();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif
