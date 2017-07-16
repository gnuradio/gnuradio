/* -*- c++ -*- */
/*
 * Copyright (C) 2002,2012,2017 Free Software Foundation, Inc.
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
 * along with this file; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/filter/mmse_interp_differentiator_cc.h>
#include "gnuradio/filter/interp_differentiator_taps.h"
#include <stdexcept>

namespace gr {
  namespace filter {

    mmse_interp_differentiator_cc::mmse_interp_differentiator_cc()
    {
      filters.resize (DNSTEPS + 1);

      for(int i = 0; i < DNSTEPS + 1; i++) {
	std::vector<float> t (&Dtaps[i][0], &Dtaps[i][DNTAPS]);
	filters[i] = new kernel::fir_filter_ccf(1, t);
      }
    }

    mmse_interp_differentiator_cc::~mmse_interp_differentiator_cc()
    {
      for(int i = 0; i < DNSTEPS + 1; i++)
	delete filters[i];
    }

    unsigned
    mmse_interp_differentiator_cc::ntaps() const
    {
      return DNTAPS;
    }

    unsigned
    mmse_interp_differentiator_cc::nsteps() const
    {
      return DNSTEPS;
    }

    gr_complex
    mmse_interp_differentiator_cc::differentiate(const gr_complex input[],
					         float mu) const
    {
      int imu = (int)rint(mu * DNSTEPS);

      if((imu < 0) || (imu > DNSTEPS)) {
	throw std::runtime_error(
                         "mmse_interp_differentiator_cc: imu out of bounds.\n");
      }

      gr_complex r = filters[imu]->filter(input);
      return r;
    }

  }  /* namespace filter */
}  /* namespace gr */
