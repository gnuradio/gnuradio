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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qa_gr_fir_fff.h>
#include <gr_fir_fff.h>
#include <gr_fir_util.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <cppunit/TestAssert.h>
#include <random.h>
#include <string.h>

using std::vector;

typedef float	i_type;
typedef float	o_type;
typedef float	tap_type;
typedef	float	acc_type;

#define	ERR_DELTA	(1e-6)

#define	NELEM(x) (sizeof (x) / sizeof (x[0]))


//
// typedef for something logically "pointer to constructor".
// there may be a better way, please let me know...
//
typedef gr_fir_fff* (*fir_maker_t)(const std::vector<tap_type> &taps);


// ----------------------------------------------------------------

const static i_type input_1[] = {
  234,  -4,  23,  -56,  45,    98,  -23,  -7
};

const static tap_type taps_1a[] = {
  -3
};

const static o_type expected_1a[] = {
  -702, 12, -69,   168, -135, -294,  69,  21
};

const static tap_type taps_1b[] = {
  -4, 5
};

const static o_type expected_1b[] = {
  1186, -112, 339, -460, -167, 582, -87
};

// ----------------------------------------------------------------

static void
test_known_io (fir_maker_t maker)
{
  vector<tap_type> t1a (&taps_1a[0], &taps_1a[NELEM (taps_1a)]);
  vector<tap_type> t1b (&taps_1b[0], &taps_1b[NELEM (taps_1b)]);
  
  gr_fir_fff *f1 = maker (t1a);		// create filter
  CPPUNIT_ASSERT_EQUAL ((unsigned) 1, f1->ntaps ());	// check ntaps

  // check filter output
  int n = NELEM (input_1) - f1->ntaps () + 1;
  for (int i = 0; i < n; i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_1a[i], f1->filter (&input_1[i]), ERR_DELTA);

  f1->set_taps (t1b);			// set new taps
  CPPUNIT_ASSERT_EQUAL ((unsigned) 2, f1->ntaps ());	// check ntaps
  
  // check filter output
  n = NELEM (input_1) - f1->ntaps () + 1;
  for (int i = 0; i < n; i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_1b[i], f1->filter (&input_1[i]), ERR_DELTA);

  // test filterN interface

  o_type output[NELEM (expected_1b)];
  memset (output, 0, sizeof (output));

  f1->filterN (output, input_1, n);
  for (int i = 0; i < n; i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_1b[i], output[i], ERR_DELTA);

  delete f1;
}

//
// Test for ntaps in [0,9], and input lengths in [0,17].
// This ensures that we are building the shifted taps correctly,
// and exercises all corner cases on input alignment and length.
//

static float
uniform ()
{
  return 2.0 * ((float) random () / RANDOM_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = rint (uniform () * 32768);
}

static o_type
ref_dotprod (const i_type input[], const tap_type taps[], int ntaps)
{
  acc_type	sum = 0;
  for (int i = 0; i < ntaps; i++)
    sum += input[i] * taps[ntaps - i - 1];

  return sum;
}

static void
test_random_io (fir_maker_t maker)  
{
  const int	MAX_TAPS	= 32;
  const int	OUTPUT_LEN	= 17;
  const int	INPUT_LEN	= MAX_TAPS + OUTPUT_LEN;

  i_type 	input[INPUT_LEN];
  o_type 	expected_output[OUTPUT_LEN];
  o_type 	actual_output[OUTPUT_LEN];
  tap_type	taps[MAX_TAPS];


  srandom (0);	// we want reproducibility

  for (int n = 0; n <= MAX_TAPS; n++){
    for (int ol = 0; ol <= OUTPUT_LEN; ol++){

      // cerr << "@@@ n:ol " << n << ":" << ol << endl;

      // build random test case
      random_floats (input, INPUT_LEN);
      random_floats (taps, MAX_TAPS);

      // compute expected output values
      for (int o = 0; o < ol; o++){
	expected_output[o] = ref_dotprod (&input[o], taps, n);
      }

      // build filter

      vector<tap_type> f1_taps (&taps[0], &taps[n]);
      gr_fir_fff *f1 = maker (f1_taps);

      // zero the output, then do the filtering
      memset (actual_output, 0, sizeof (actual_output));
      f1->filterN (actual_output, input, ol);

      // check results
      //
      // we use a sloppy error margin because on the x86 architecture,
      // our reference implementation is using 80 bit floating point
      // arithmetic, while the SSE version is using 32 bit float point
      // arithmetic.
      
      for (int o = 0; o < ol; o++){
	CPPUNIT_ASSERT_DOUBLES_EQUAL (expected_output[o], actual_output[o],
				      fabs (expected_output[o]) * 9e-3);
      }

      delete f1;
    }
  }
}


static void
for_each (void (*f)(fir_maker_t))
{
  std::vector<gr_fir_fff_info>		info;
  gr_fir_util::get_gr_fir_fff_info (&info);	// get all known fff implementations 

  for (std::vector<gr_fir_fff_info>::iterator p = info.begin ();
       p != info.end ();
       ++p){

    std::cerr << " [" << p->name << "]";
    f (p->create);
  }

  std::cerr << std::endl;
}

void
qa_gr_fir_fff::t1 ()
{
  for_each (test_known_io);
}

void
qa_gr_fir_fff::t2 ()
{
  for_each (test_random_io);
}
