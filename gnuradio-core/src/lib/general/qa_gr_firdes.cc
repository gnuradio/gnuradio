/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <qa_gr_firdes.h>
#include <gr_firdes.h>
#include <cppunit/TestAssert.h>
#include <gr_complex.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

#define	NELEM(x) (sizeof (x) / sizeof (x[0]))

using std::vector;

static void
print_taps (std::ostream &s, vector<float> &v)
{
  
  for (unsigned int i = 0; i < v.size (); i++){
    printf ("tap[%2d] = %16.7e\n", i, v[i]);
  }
}

static void
check_symmetry (vector<float> &v)
{
  int	n = v.size ();
  int	m = n / 2;

  for (int i = 0; i < m; i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (v[i], v[n - i - 1], 1e-9);
}

const static float t1_exp[53] = {
 -9.0525491e-04,
  2.0713841e-04,
  1.2388536e-03,
  2.9683491e-04,
 -1.7744775e-03,
 -1.3599906e-03,
  2.2031884e-03,
  3.2744040e-03,
 -1.8868084e-03,
 -5.9935520e-03,
  6.4301129e-18,
  8.9516686e-03,
  4.2178580e-03,
 -1.0998557e-02,
 -1.1173409e-02,
  1.0455756e-02,
  2.0686293e-02,
 -5.2032238e-03,
 -3.1896964e-02,
 -7.4998410e-03,
  4.3362070e-02,
  3.2502845e-02,
 -5.3328082e-02,
 -8.5621715e-02,
  6.0117975e-02,
  3.1128189e-01,
  4.3769023e-01,
  3.1128189e-01,
  6.0117975e-02,
 -8.5621715e-02,
 -5.3328082e-02,
  3.2502845e-02,
  4.3362070e-02,
 -7.4998410e-03,
 -3.1896964e-02,
 -5.2032238e-03,
  2.0686293e-02,
  1.0455756e-02,
 -1.1173409e-02,
 -1.0998557e-02,
  4.2178580e-03,
  8.9516686e-03,
  6.4301129e-18,
 -5.9935520e-03,
 -1.8868084e-03,
  3.2744040e-03,
  2.2031884e-03,
 -1.3599906e-03,
 -1.7744775e-03,
  2.9683491e-04,
  1.2388536e-03,
  2.0713841e-04,
 -9.0525491e-04
};

const static float t2_exp[53] = {
  9.0380036e-04,
 -2.0680559e-04,
 -1.2368630e-03,
 -2.9635796e-04,
  1.7716263e-03,
  1.3578053e-03,
 -2.1996482e-03,
 -3.2691427e-03,
  1.8837767e-03,
  5.9839217e-03,
 -6.4197810e-18,
 -8.9372853e-03,
 -4.2110807e-03,
  1.0980885e-02,
  1.1155456e-02,
 -1.0438956e-02,
 -2.0653054e-02,
  5.1948633e-03,
  3.1845711e-02,
  7.4877902e-03,
 -4.3292396e-02,
 -3.2450620e-02,
  5.3242393e-02,
  8.5484132e-02,
 -6.0021374e-02,
 -3.1078172e-01,
  5.6184036e-01,
 -3.1078172e-01,
 -6.0021374e-02,
  8.5484132e-02,
  5.3242393e-02,
 -3.2450620e-02,
 -4.3292396e-02,
  7.4877902e-03,
  3.1845711e-02,
  5.1948633e-03,
 -2.0653054e-02,
 -1.0438956e-02,
  1.1155456e-02,
  1.0980885e-02,
 -4.2110807e-03,
 -8.9372853e-03,
 -6.4197810e-18,
  5.9839217e-03,
  1.8837767e-03,
 -3.2691427e-03,
 -2.1996482e-03,
  1.3578053e-03,
  1.7716263e-03,
 -2.9635796e-04,
 -1.2368630e-03,
 -2.0680559e-04,
  9.0380036e-04
};

const static float t3_exp[107] = {
  -1.8970841e-06,
  -7.1057165e-04,
   5.4005696e-04,
   4.6233178e-04,
   2.0572044e-04,
   3.5209916e-04,
  -1.4098573e-03,
   1.1279077e-04,
  -6.2994129e-04,
   1.1450432e-03,
   1.3637283e-03,
  -6.4360141e-04,
   3.6509900e-04,
  -3.2864159e-03,
   7.0192874e-04,
   3.7524730e-04,
   2.0256115e-03,
   3.0641893e-03,
  -3.6618244e-03,
   7.5592739e-05,
  -5.5586505e-03,
   2.3849572e-03,
   4.0114378e-03,
   1.6636450e-03,
   4.7835698e-03,
  -1.0191196e-02,
  -3.8158931e-04,
  -5.5551580e-03,
   5.3901658e-03,
   1.1366769e-02,
  -3.0000482e-03,
   4.9341680e-03,
  -2.0093076e-02,
   5.5752542e-17,
   1.2093617e-03,
   8.6089745e-03,
   2.2382140e-02,
  -1.6854567e-02,
   1.6913920e-03,
  -3.1222520e-02,
   3.2711059e-03,
   2.2604836e-02,
   8.1451107e-03,
   3.7583180e-02,
  -5.2293688e-02,
  -8.0551542e-03,
  -4.0092729e-02,
   1.5582236e-02,
   9.7452506e-02,
  -1.6183170e-02,
   8.3281815e-02,
  -2.8196752e-01,
  -1.0965768e-01,
   5.2867508e-01,
  -1.0965768e-01,
  -2.8196752e-01,
   8.3281815e-02,
  -1.6183170e-02,
   9.7452506e-02,
   1.5582236e-02,
  -4.0092729e-02,
  -8.0551542e-03,
  -5.2293688e-02,
   3.7583180e-02,
   8.1451107e-03,
   2.2604836e-02,
   3.2711059e-03,
  -3.1222520e-02,
   1.6913920e-03,
  -1.6854567e-02,
   2.2382140e-02,
   8.6089745e-03,
   1.2093617e-03,
   5.5752542e-17,
  -2.0093076e-02,
   4.9341680e-03,
  -3.0000482e-03,
   1.1366769e-02,
   5.3901658e-03,
  -5.5551580e-03,
  -3.8158931e-04,
  -1.0191196e-02,
   4.7835698e-03,
   1.6636450e-03,
   4.0114378e-03,
   2.3849572e-03,
  -5.5586505e-03,
   7.5592739e-05,
  -3.6618244e-03,
   3.0641893e-03,
   2.0256115e-03,
   3.7524730e-04,
   7.0192874e-04,
  -3.2864159e-03,
   3.6509900e-04,
  -6.4360141e-04,
   1.3637283e-03,
   1.1450432e-03,
  -6.2994129e-04,
   1.1279077e-04,
  -1.4098573e-03,
   3.5209916e-04,
   2.0572044e-04,
   4.6233178e-04,
   5.4005696e-04,
  -7.1057165e-04,
  -1.8970841e-06
};


void
qa_gr_firdes::t1 ()
{
  vector<float> taps =
    gr_firdes::low_pass ( 1.0,
			  8000,
			  1750,
			  500,
			  gr_firdes::WIN_HAMMING);

  // cout << "ntaps: " << taps.size () << endl;
  // print_taps (cout, taps);

  CPPUNIT_ASSERT_EQUAL (NELEM (t1_exp), taps.size ());
  for (unsigned int i = 0; i < taps.size (); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (t1_exp[i], taps[i], 1e-9);

  check_symmetry (taps);
}

void
qa_gr_firdes::t2 ()
{
  vector<float> taps =
    gr_firdes::high_pass ( 1.0,
			   8000,
			   1750,
			   500,
			   gr_firdes::WIN_HAMMING);

  // cout << "ntaps: " << taps.size () << endl;
  // print_taps (cout, taps);

  CPPUNIT_ASSERT_EQUAL (NELEM (t2_exp), taps.size ());

  for (unsigned int i = 0; i < taps.size (); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (t2_exp[i], taps[i], 1e-9);

  check_symmetry (taps);
}

void
qa_gr_firdes::t3 ()
{
  vector<float> taps =
    gr_firdes::band_pass ( 1.0,
			   20e6,
			   5.75e6 - (5.28e6/2),
			   5.75e6 + (5.28e6/2),
			   0.62e6,
			   gr_firdes::WIN_HAMMING);

  // cout << "ntaps: " << taps.size () << endl;
  // print_taps (cout, taps);

  CPPUNIT_ASSERT_EQUAL (NELEM (t3_exp), taps.size ());

  for (unsigned int i = 0; i < taps.size (); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (t3_exp[i], taps[i], 1e-7);

  check_symmetry (taps);
}

void
qa_gr_firdes::t4 ()
{
}
