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

#include "frequency_modulator_fc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/fxpt.h>
#include <math.h>
#include <boost/math/special_functions/trunc.hpp>

namespace gr {
  namespace analog {

    frequency_modulator_fc::sptr
    frequency_modulator_fc::make(double sensitivity)
    {
      return gnuradio::get_initial_sptr
	(new frequency_modulator_fc_impl(sensitivity));
    }

    frequency_modulator_fc_impl::frequency_modulator_fc_impl(double sensitivity)
      : sync_block("frequency_modulator_fc",
		      io_signature::make(1, 1, sizeof(float)),
		      io_signature::make(1, 1, sizeof(gr_complex))),
	d_sensitivity(sensitivity), d_phase(0)
    {
    }

    frequency_modulator_fc_impl::~frequency_modulator_fc_impl()
    {
    }

    int
    frequency_modulator_fc_impl::work(int noutput_items,
				      gr_vector_const_void_star &input_items,
				      gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      for(int i = 0; i < noutput_items; i++) {
	d_phase = d_phase + d_sensitivity * in[i];

	while(d_phase > (float)(M_PI))
	  d_phase -= (float)(2.0 * M_PI);
	while(d_phase < (float)(-M_PI))
	  d_phase += (float)(2.0 * M_PI);

	float oi, oq;

	gr_int32 angle = gr::fxpt::float_to_fixed (d_phase);
	gr::fxpt::sincos(angle, &oq, &oi);
	out[i] = gr_complex(oi, oq);
      }

      return noutput_items;
    }

  } /* namespace analog */
} /* namespace gr */
