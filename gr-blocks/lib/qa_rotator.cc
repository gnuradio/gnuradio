/* -*- c++ -*- */
/*
 * Copyright 2002,2013 Free Software Foundation, Inc.
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

#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include <qa_rotator.h>
#include <gnuradio/blocks/rotator.h>
#include <stdio.h>
#include <cmath>
#include <gnuradio/expj.h>

// error vector magnitude
__GR_ATTR_UNUSED static float
error_vector_mag(gr_complex a, gr_complex b)
{
  return abs(a-b);
}

void
qa_rotator::t1()
{
  static const unsigned	int N = 100000;

  gr::blocks::rotator r;

  double phase_incr = 2*M_PI / 1003;
  double phase = 0;

  // Old code: We increment then return the rotated value, thus we
  // need to start one tick back r.set_phase(gr_complex(1,0) *
  // conj(gr_expj(phase_incr)));

  r.set_phase(gr_complex(1,0));
  r.set_phase_incr(gr_expj(phase_incr));

  for(unsigned i = 0; i < N; i++) {
    gr_complex expected = gr_expj(phase);
    gr_complex actual = r.rotate(gr_complex(1, 0));

#if 0
    float evm = error_vector_mag(expected, actual);
    printf("[%6d] expected: (%8.6f, %8.6f)  actual: (%8.6f, %8.6f)  evm: %8.6f\n",
	   i, expected.real(), expected.imag(), actual.real(), actual.imag(), evm);
#endif

    CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected, actual, 0.0001);

    phase += phase_incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
}

void
qa_rotator::t2()
{
  static const unsigned	int N = 100000;

  gr::blocks::rotator r;
  gr_complex *input  = new gr_complex[N];
  gr_complex *output = new gr_complex[N];

  double phase_incr = 2*M_PI / 1003;
  double phase = 0;

  r.set_phase(gr_complex(1,0));
  r.set_phase_incr(gr_expj(phase_incr));

  // Generate a unity sequence
  for(unsigned i = 0; i < N; i++)
    input[i] = gr_complex(1.0f, 0.0f);

  // Rotate it
  r.rotateN(output, input, N);

  // Compare with expected result
  for(unsigned i = 0; i < N; i++) {
    gr_complex expected = gr_expj(phase);
    gr_complex actual   = output[i];

#if 0
    float evm = error_vector_mag(expected, actual);
    printf("[%6d] expected: (%8.6f, %8.6f)  actual: (%8.6f, %8.6f)  evm: %8.6f\n",
	   i, expected.real(), expected.imag(), actual.real(), actual.imag(), evm);
#endif

    CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected, actual, 0.0001);

    phase += phase_incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }

  delete[] output;
  delete[] input;
}
