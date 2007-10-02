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

/*
 * FIXME.  This code is virtually identical to qa_gr_fir_?CC.cc
 *   Kludge up some kind of macro to handle the minor differences.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gr_types.h>

typedef short		i_type;
typedef gr_complex	o_type;
typedef gr_complex	tap_type;
typedef	gr_complex	acc_type;


#include <qa_gr_fir_scc.h>
#include <gr_fir_scc.h>
#include <gr_fir_util.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <gr_types.h>
#include <cppunit/TestAssert.h>
#include <random.h>

using std::vector;

#define	ERR_DELTA	(1e-5)

#define	NELEM(x) (sizeof (x) / sizeof (x[0]))

//
// typedef for something logically "pointer to constructor".
// there may be a better way, please let me know...
//
typedef gr_fir_scc* (*fir_maker_t)(const std::vector<tap_type> &taps);


static float
uniform ()
{
  return 2.0 * ((float) random () / RANDOM_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_input (i_type *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = (i_type) rint (uniform () * 32767);
}

static void
random_complex (gr_complex *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++){
    float re = rint (uniform () * 32767);
    float im = rint (uniform () * 32767);
    buf[i] = gr_complex (re, im);
  }
}

static o_type
ref_dotprod (const i_type input[], const tap_type taps[], int ntaps)
{
  acc_type	sum = 0;
  for (int i = 0; i < ntaps; i++)
    sum += (float) input[i] * taps[ntaps - i - 1];

  return sum;
}

//
// Test for ntaps in [0,9], and input lengths in [0,17].
// This ensures that we are building the shifted taps correctly,
// and exercises all corner cases on input alignment and length.
//

static void
test_random_io (fir_maker_t maker)  
{
  const int	MAX_TAPS	= 9;
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
      random_input (input, INPUT_LEN);
      random_complex (taps, MAX_TAPS);

      // compute expected output values
      for (int o = 0; o < ol; o++){
	expected_output[o] = ref_dotprod (&input[o], taps, n);
      }

      // build filter

      vector<tap_type> f1_taps (&taps[0], &taps[n]);
      gr_fir_scc *f1 = maker (f1_taps);

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
	CPPUNIT_ASSERT_COMPLEXES_EQUAL(expected_output[o],
				       actual_output[o],
				       abs (expected_output[o]) * ERR_DELTA);
      }

      delete f1;
    }
  }
}

static void
for_each (void (*f)(fir_maker_t))
{
  std::vector<gr_fir_scc_info>		info;
  gr_fir_util::get_gr_fir_scc_info (&info);	// get all known scc implementations 

  for (std::vector<gr_fir_scc_info>::iterator p = info.begin ();
       p != info.end ();
       ++p){

    std::cerr << " [" << p->name << "]";
    f (p->create);
  }

  std::cerr << std::endl;
}

void
qa_gr_fir_scc::t1 ()
{
  for_each (test_random_io);
}
