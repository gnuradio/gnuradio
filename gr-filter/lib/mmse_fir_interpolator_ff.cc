/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/filter/interpolator_taps.h>
#include <stdexcept>

namespace gr {
  namespace filter {

    mmse_fir_interpolator_ff::mmse_fir_interpolator_ff()
    {
      filters.resize(NSTEPS + 1);

      for(int i = 0; i < NSTEPS + 1; i++) {
	std::vector<float> t(&taps[i][0], &taps[i][NTAPS]);
	filters[i] = new kernel::fir_filter_fff(1, t);
      }
    }

    mmse_fir_interpolator_ff::~mmse_fir_interpolator_ff()
    {
      for(int i = 0; i < NSTEPS + 1; i++)
	delete filters[i];
    }

    unsigned
    mmse_fir_interpolator_ff::ntaps() const
    {
      return NTAPS;
    }

    unsigned
    mmse_fir_interpolator_ff::nsteps() const
    {
      return NSTEPS;
    }

    float
    mmse_fir_interpolator_ff::interpolate(const float input[],
					  float mu) const
    {
      int imu = (int)rint(mu * NSTEPS);

      if((imu < 0) || (imu > NSTEPS)) {
	throw std::runtime_error("mmse_fir_interpolator_ff: imu out of bounds.\n");
      }

      float r = filters[imu]->filter(input);
      return r;
    }

  }  /* namespace filter */
}  /* namespace gr */
