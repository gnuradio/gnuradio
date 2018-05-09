/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2012 Free Software Foundation, Inc.
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

#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <gnuradio/fft/fft.h>
#include <volk/volk.h>
#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <cmath>
#include <stdexcept>
#include <stdint.h>

namespace gr {
  namespace filter {

    static float
    test_fcn_sin(double index)
    {
      return (2 * sin (index * 0.25 * 2 * M_PI + 0.125 * M_PI)
	      + 3 * sin (index * 0.077 * 2 * M_PI + 0.3 * M_PI));
    }

    static float
    test_fcn_cos(double index)
    {
      return (2 * cos (index * 0.25 * 2 * M_PI + 0.125 * M_PI)
	      + 3 * cos (index * 0.077 * 2 * M_PI + 0.3 * M_PI));
    }

    static gr_complex
    test_fcn(double index)
    {
      return gr_complex(test_fcn_cos(index), test_fcn_sin(index));
    }


    BOOST_AUTO_TEST_CASE(t1)
    {
      static const unsigned N = 100;
      gr_complex *input = (gr_complex*)volk_malloc((N + 10)*sizeof(gr_complex),
                                                   volk_get_alignment());

      for(unsigned i = 0; i < N+10; i++)
	input[i] = test_fcn((double) i);

      mmse_fir_interpolator_cc intr;
      float inv_nsteps = 1.0 / intr.nsteps();

      for(unsigned i = 0; i < N; i++) {
	for(unsigned imu = 0; imu <= intr.nsteps (); imu += 1) {
	  gr_complex expected = test_fcn((i + 3) + imu * inv_nsteps);
	  gr_complex actual = intr.interpolate(&input[i], imu * inv_nsteps);

	  BOOST_CHECK(std::abs(expected - actual) <= 0.004);
	  // printf ("%9.6f  %9.6f  %9.6f\n", expected, actual, expected - actual);
	}
      }
      volk_free(input);
    }

    /*
     * Force bad alignment and confirm that it raises an exception
     */
    void t2_body()
    {
      static const unsigned N = 100;
      float float_input[2*(N+10) + 1];
      gr_complex *input;

      // We require that gr_complex be aligned on an 8-byte boundary.
      // Ensure that we ARE NOT ;)

      if(((intptr_t) float_input & 0x7) == 0)
	input = reinterpret_cast<gr_complex *>(&float_input[1]);
      else
	input = reinterpret_cast<gr_complex *>(&float_input[0]);

      for(unsigned i = 0; i < (N+10); i++)
	input[i] = test_fcn((double) i);

      mmse_fir_interpolator_cc intr;
      float inv_nsteps = 1.0 / intr.nsteps();

      //for(unsigned i = 0; i < N; i++) {
      for(unsigned i = 0; i < 1; i++) {
	for(unsigned imu = 0; imu <= intr.nsteps (); imu += 1) {
	  gr_complex expected = test_fcn((i + 3) + imu * inv_nsteps);
	  gr_complex actual = intr.interpolate(&input[i], imu * inv_nsteps);

	  BOOST_CHECK(std::abs(expected - actual) <= 0.004);
	  // printf ("%9.6f  %9.6f  %9.6f\n", expected, actual, expected - actual);
	}
      }
    }

    //BOOST_AUTO_TEST_CASE(t2_throw)
    //{
      //BOOST_REQUIRE_THROW(t2_body(), std::invalid_argument);
    //}

  } /* namespace filter */
} /* namespace gr */
