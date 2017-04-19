/* -*- c++ -*- */
/*
 * Copyright (C) 2002,2007,2012,2017 Free Software Foundation, Inc.
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

#include <cppunit/TestAssert.h>
#include <qa_mmse_interp_differentiator_cc.h>
#include <gnuradio/filter/mmse_interp_differentiator_cc.h>
#include <gnuradio/fft/fft.h>
#include <volk/volk.h>
#include <cstdio>
#include <cmath>
#include <stdexcept>
#include <stdint.h>

namespace gr {
  namespace filter {

    static const double k1 = 0.25 * 2 * M_PI;
    static const double k2 = 0.125 * M_PI;
    static const double k3 = 0.077 * 2 * M_PI;
    static const double k4 = 0.3 * M_PI;

    static double
    phase_wrap(double arg)
    {
      while (arg >= 2.0*M_PI)
        arg -= 2.0*M_PI;
      while (arg <= 2.0*M_PI)
        arg += 2.0*M_PI;
      return arg;
    }

    static float
    test_fcn_sin(double index)
    {
      double arg1 = phase_wrap(index * k1 + k2);
      double arg2 = phase_wrap(index * k3 + k4);

      return (2 * sin (arg1) + 3 * sin (arg2));
    }

    static float
    test_fcn_cos(double index)
    {
      double arg1 = phase_wrap(index * k1 + k2);
      double arg2 = phase_wrap(index * k3 + k4);

      return (2 * cos (arg1) + 3 * cos (arg2));
    }

    static float
    test_fcn_dsin(double index)
    {
      double arg1 = phase_wrap(index * k1 + k2);
      double arg2 = phase_wrap(index * k3 + k4);

      return (k1 * 2 * cos (arg1) + k3 * 3 * cos (arg2));
    }

    static float
    test_fcn_dcos(double index)
    {
      double arg1 = phase_wrap(index * k1 + k2);
      double arg2 = phase_wrap(index * k3 + k4);

      return (-k1 * 2 * sin (arg1) - k3 * 3 * sin (arg2));
    }

    static gr_complex
    test_fcn(double index)
    {
      return gr_complex(test_fcn_cos(index), test_fcn_sin(index));
    }

    static gr_complex
    test_fcn_d(double index)
    {
      return gr_complex(test_fcn_dcos(index), test_fcn_dsin(index));
    }

    void
    qa_mmse_interp_differentiator_cc::t1()
    {
      static const unsigned N = 100;
      gr_complex *input = (gr_complex*)volk_malloc((N + 10)*sizeof(gr_complex),
                                                   volk_get_alignment());

      for(unsigned i = 0; i < N+10; i++)
	input[i] = test_fcn((double) i);

      mmse_interp_differentiator_cc diffr;
      float inv_nsteps = 1.0 / diffr.nsteps();

      for(unsigned i = 0; i < N; i++) {
	for(unsigned imu = 0; imu <= diffr.nsteps (); imu += 1) {
	  gr_complex expected = test_fcn_d((i + 3) + imu * inv_nsteps);
	  gr_complex actual = diffr.differentiate(&input[i], imu * inv_nsteps);

	  CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected, actual, 0.0103);
	  // printf ("%9.6f  %9.6f  %9.6f \n", expected.real(), actual.real(), expected.real() - actual.real());
	  // printf ("%9.6f  %9.6f  %9.6f \n", expected.imag(), actual.imag(), expected.imag() - actual.imag());
	}
      }
      volk_free(input);
    }

  } /* namespace filter */
} /* namespace gr */
