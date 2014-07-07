/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef	INCLUDED_WAVELET_SQUASH_FF_IMPL_H
#define	INCLUDED_WAVELET_SQUASH_FF_IMPL_H

#include <gnuradio/wavelet/api.h>
#include <gnuradio/wavelet/squash_ff.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

namespace gr {
  namespace wavelet {

    class WAVELET_API squash_ff_impl : public squash_ff
    {
      size_t  d_inum;
      size_t  d_onum;
      double *d_igrid;
      double *d_iwork;
      double *d_ogrid;

      gsl_interp_accel *d_accel;
      gsl_spline       *d_spline;

    public:
      squash_ff_impl(const std::vector<float> &igrid,
		     const std::vector<float> &ogrid);

      ~squash_ff_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WAVELET_FF_IMPL_H */
