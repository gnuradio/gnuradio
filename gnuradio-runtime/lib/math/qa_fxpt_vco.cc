/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2013 Free Software Foundation, Inc.
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

#include <qa_fxpt_vco.h>
#include <gnuradio/fxpt_vco.h>
#include <vco.h>
#include <cppunit/TestAssert.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

static const float SIN_COS_TOLERANCE = 1e-5;

static const float SIN_COS_K = 0.42;
static const float SIN_COS_AMPL = 0.8;

static const int SIN_COS_BLOCK_SIZE = 100000;

static double max_d(double a, double b)
{
  return fabs(a) > fabs(b) ? a : b;
}

void
qa_fxpt_vco::t0()
{
  gr::vco<float,float>	ref_vco;
  gr::fxpt_vco		new_vco;
  double max_error = 0, max_phase_error = 0;
  float			input[SIN_COS_BLOCK_SIZE];

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    input[i] = sin(double(i));
  }

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    float ref_cos = ref_vco.cos();
    float new_cos = new_vco.cos();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_cos, new_cos, SIN_COS_TOLERANCE);

    max_error = max_d(max_error, ref_cos-new_cos);

    ref_vco.adjust_phase(input[i]);
    new_vco.adjust_phase(input[i]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_vco.get_phase(), new_vco.get_phase(), SIN_COS_TOLERANCE);

    max_phase_error = max_d(max_phase_error, ref_vco.get_phase()-new_vco.get_phase());
  }
  // printf ("Fxpt  max error %.9f, max phase error %.9f\n", max_error, max_phase_error);
}

void
qa_fxpt_vco::t1()
{
  gr::vco<float,float>	ref_vco;
  gr::fxpt_vco		new_vco;
  float		*ref_block = new float[SIN_COS_BLOCK_SIZE];
  float		*new_block = new float[SIN_COS_BLOCK_SIZE];
  float		*input = new float[SIN_COS_BLOCK_SIZE];
  double max_error = 0;

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    input[i] = sin(double(i));
  }

  ref_vco.cos(ref_block, input, SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);
  new_vco.cos(new_block, input, SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_block[i], new_block[i], SIN_COS_TOLERANCE);
    max_error = max_d(max_error, ref_block[i]-new_block[i]);
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_vco.get_phase(), new_vco.get_phase(), SIN_COS_TOLERANCE);
  // printf ("Fxpt  max error %.9f, max phase error %.9f\n", max_error, ref_vco.get_phase()-new_vco.get_phase());
  delete[] ref_block;
  delete[] new_block;
  delete[] input;
}

void
qa_fxpt_vco::t2()
{
  gr::vco<gr_complex,float> ref_vco;
  gr::fxpt_vco		new_vco;
  gr_complex		*ref_block = new gr_complex[SIN_COS_BLOCK_SIZE];
  gr_complex		*new_block = new gr_complex[SIN_COS_BLOCK_SIZE];
  float		*input = new float[SIN_COS_BLOCK_SIZE];
  double max_error = 0;

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    input[i] = sin(double(i));
  }

  ref_vco.sincos(ref_block, input, SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);
  new_vco.sincos(new_block, input, SIN_COS_BLOCK_SIZE, SIN_COS_K, SIN_COS_AMPL);

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    CPPUNIT_ASSERT_COMPLEXES_EQUAL(ref_block[i], new_block[i], SIN_COS_TOLERANCE);
    max_error = max_d(max_error, abs(ref_block[i]-new_block[i]));
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_vco.get_phase(), new_vco.get_phase(), SIN_COS_TOLERANCE);
  // printf ("Fxpt  max error %.9f, max phase error %.9f\n", max_error, ref_vco.get_phase()-new_vco.get_phase());
  delete[] ref_block;
  delete[] new_block;
  delete[] input;
}

void
qa_fxpt_vco::t3()
{
}
