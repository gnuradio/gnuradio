/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdexcept>
#include <squash_ff_impl.h>
#include <gnuradio/io_signature.h>

// expect input vector of igrid.size y-values,
// produce output vector of ogrid.size y-values

namespace gr {
  namespace wavelet {

    squash_ff::sptr squash_ff::make(const std::vector<float> &igrid,
				    const std::vector<float> &ogrid)
    {
      return gnuradio::get_initial_sptr(new squash_ff_impl(igrid, ogrid));
    }

    squash_ff_impl::squash_ff_impl(const std::vector<float> &igrid,
				   const std::vector<float> &ogrid)
      : sync_block("squash_ff",
		      io_signature::make(1, 1, sizeof(float) * igrid.size()),
		      io_signature::make(1, 1, sizeof(float) * ogrid.size()))
    {
      d_inum  = igrid.size();
      d_onum  = ogrid.size();
      d_igrid = (double *) malloc(d_inum * sizeof(double));
      d_iwork = (double *) malloc(d_inum * sizeof(double));
      d_ogrid = (double *) malloc(d_onum * sizeof(double));
      for (unsigned int i = 0; i < d_inum; i++)
	d_igrid[i] = igrid[i];
      for (unsigned int i = 0; i < d_onum; i++)
	d_ogrid[i] = ogrid[i];

      d_accel = gsl_interp_accel_alloc();
      d_spline = gsl_spline_alloc(gsl_interp_cspline, d_inum);	// FIXME check w/ Frank
    }

    squash_ff_impl::~squash_ff_impl()
    {
      free((char *) d_igrid);
      free((char *) d_iwork);
      free((char *) d_ogrid);
      gsl_interp_accel_free(d_accel);
      gsl_spline_free(d_spline);
    }

    int
    squash_ff_impl::work(int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
    {
      const float *in  = (const float *) input_items[0];
      float       *out = (float *) output_items[0];

      for (int count = 0; count < noutput_items; count++) {

	for (unsigned int i = 0; i < d_inum; i++)
	  d_iwork[i] = in[i];

	gsl_spline_init(d_spline, d_igrid, d_iwork, d_inum);

	for (unsigned int i = 0; i < d_onum; i++)
	  out[i] = gsl_spline_eval(d_spline, d_ogrid[i], d_accel);

	in  += d_inum;
	out += d_onum;
      }

      return noutput_items;
    }

  } /* namespace wavelet */
} /* namespace gr */
