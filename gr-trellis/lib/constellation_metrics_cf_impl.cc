/* -*- c++ -*- */
/*
 * Copyright 2004,2010-2012 Free Software Foundation, Inc.
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

#include "constellation_metrics_cf_impl.h"
#include <gnuradio/io_signature.h>
#include <assert.h>
#include <stdexcept>
#include <iostream>

namespace gr {
  namespace trellis {

    constellation_metrics_cf::sptr
    constellation_metrics_cf::make(digital::constellation_sptr constellation,
				   digital::trellis_metric_type_t TYPE)
    {
      return gnuradio::get_initial_sptr
	(new constellation_metrics_cf_impl(constellation, TYPE));
    }

    constellation_metrics_cf_impl::constellation_metrics_cf_impl(digital::constellation_sptr constellation,
								 digital::trellis_metric_type_t TYPE)
      : block("constellation_metrics_cf",
		 io_signature::make(1, -1, sizeof(gr_complex)),
		 io_signature::make(1, -1, sizeof(float))),
	d_constellation(constellation),
	d_TYPE(TYPE),
	d_O(constellation->arity()),
	d_D(constellation->dimensionality())
    {
      set_relative_rate(1.0 * d_O / ((double) d_D));
      set_output_multiple((int)d_O);
    }

    constellation_metrics_cf_impl::~constellation_metrics_cf_impl()
    {
    }

    void
    constellation_metrics_cf_impl::forecast(int noutput_items,
					    gr_vector_int &ninput_items_required)
    {
      assert(noutput_items % d_O == 0);

      unsigned int input_required =  d_D * noutput_items / d_O;
      unsigned int ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++)
	ninput_items_required[i] = input_required;
    }

    int
    constellation_metrics_cf_impl::general_work(int noutput_items,
						gr_vector_int &ninput_items,
						gr_vector_const_void_star &input_items,
						gr_vector_void_star &output_items)
    {
      unsigned int nstreams = input_items.size();

      for(unsigned int m=0;m<nstreams;m++) {
	const gr_complex *in = (gr_complex*)input_items[m];
	float *out = (float*)output_items[m];

	for(unsigned int i = 0; i < noutput_items / d_O ; i++) {
	  d_constellation->calc_metric(&(in[i*d_D]), &(out[i*d_O]), d_TYPE);
	}
      }

      consume_each(d_D * noutput_items / d_O);
      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
