/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#include <qa_fxpt_nco.h>
#include <gnuradio/fxpt_nco.h>
#include <gnuradio/nco.h>
#include <cppunit/TestAssert.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

static const float SIN_COS_TOLERANCE = 1e-5;

//static const float SIN_COS_FREQ = 5003;
static const float SIN_COS_FREQ = 4096;

static const int SIN_COS_BLOCK_SIZE = 100000;

static double max_d(double a, double b)
{
  return fabs(a) > fabs(b) ? a : b;
}

void
qa_fxpt_nco::t0()
{
  gr::nco<float,float>	ref_nco;
  gr::fxpt_nco		new_nco;
  double max_error = 0, max_phase_error = 0;

  ref_nco.set_freq((float)(2 * M_PI / SIN_COS_FREQ));
  new_nco.set_freq((float)(2 * M_PI / SIN_COS_FREQ));

  CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_nco.get_freq(), new_nco.get_freq(), SIN_COS_TOLERANCE);

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    float ref_sin = ref_nco.sin();
    float new_sin = new_nco.sin();
    //printf ("i = %6d\n", i);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_sin, new_sin, SIN_COS_TOLERANCE);

    max_error = max_d(max_error, ref_sin-new_sin);

    float ref_cos = ref_nco.cos();
    float new_cos = new_nco.cos();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_cos, new_cos, SIN_COS_TOLERANCE);

    max_error = max_d(max_error, ref_cos-new_cos);

    ref_nco.step();
    new_nco.step();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_nco.get_phase(), new_nco.get_phase(), SIN_COS_TOLERANCE);

    max_phase_error = max_d(max_phase_error, ref_nco.get_phase()-new_nco.get_phase());
  }
  // printf ("Fxpt  max error %.9f, max phase error %.9f\n", max_error, max_phase_error);
}

void
qa_fxpt_nco::t1()
{
  gr::nco<float,float>	ref_nco;
  gr::fxpt_nco		new_nco;
  gr_complex*		ref_block = new gr_complex[SIN_COS_BLOCK_SIZE];
  gr_complex*		new_block = new gr_complex[SIN_COS_BLOCK_SIZE];
  double max_error = 0;

  ref_nco.set_freq((float)(2 * M_PI / SIN_COS_FREQ));
  new_nco.set_freq((float)(2 * M_PI / SIN_COS_FREQ));

  CPPUNIT_ASSERT_DOUBLES_EQUAL(ref_nco.get_freq(), new_nco.get_freq(), SIN_COS_TOLERANCE);

  ref_nco.sincos((gr_complex*)ref_block, SIN_COS_BLOCK_SIZE);
  new_nco.sincos((gr_complex*)new_block, SIN_COS_BLOCK_SIZE);

  for(int i = 0; i < SIN_COS_BLOCK_SIZE; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref_block[i].real(), new_block[i].real(), SIN_COS_TOLERANCE);
    max_error = max_d (max_error, ref_block[i].real()-new_block[i].real());

    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref_block[i].imag(), new_block[i].imag(), SIN_COS_TOLERANCE);
    max_error = max_d (max_error, ref_block[i].imag()-new_block[i].imag());
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL (ref_nco.get_phase(), new_nco.get_phase(), SIN_COS_TOLERANCE);
  // printf ("Fxpt  max error %.9f, max phase error %.9f\n", max_error, max_phase_error);
  delete[] ref_block;
  delete[] new_block;
}

void
qa_fxpt_nco::t2()
{
}

void
qa_fxpt_nco::t3()
{
}
