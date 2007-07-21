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

#include <cppunit/TestAssert.h>
#include <qa_complex_dotprod_x86.h>
#include <complex_dotprod_x86.h>
#include <string.h>
#include <iostream>
#include <malloc16.h>
#include <sse_debug.h>
#include <cmath>
#include <gr_cpu.h>
#include <random.h>

using std::cerr;

/// Macro for primitive value comparisons
#define assertcomplexEqual(expected0,expected1,actual,delta)	\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (expected0, actual[0], delta);	\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (expected1, actual[1], delta);	


#define	MAX_BLKS	10
#define	FLOATS_PER_BLK	 4
#define	SHORTS_PER_BLK	 2

#define	ERR_DELTA	(1e-6)

static float
uniform ()
{
  return 2.0 * ((float) random () / RANDOM_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = rint (uniform () * 32767);
}

static void
zero_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = 0.0;
}

static void
random_shorts (short *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = (short) rint (uniform () * 32767);
}

static void
zero_shorts (short *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = 0;
}

void
ref_complex_dotprod (const short *input,
		   const float *taps, unsigned n_2_complex_blocks,
		   float *result)
{
  float sum0[2] = {0,0};
  float sum1[2] = {0,0};

  do {

    sum0[0] += input[0] * taps[0];
    sum0[1] += input[0] * taps[1];
    sum1[0] += input[1] * taps[2];
    sum1[1] += input[1] * taps[3];

    input += 2;
    taps += 4;

  } while (--n_2_complex_blocks != 0);


  result[0] = sum0[0] + sum1[0];
  result[1] = sum0[1] + sum1[1];
}

void 
qa_complex_dotprod_x86::setUp ()
{
  taps = (float *) calloc16Align (MAX_BLKS, 
				  sizeof (float) * FLOATS_PER_BLK);

  input = (short *) calloc16Align (MAX_BLKS,
				   sizeof (short) * SHORTS_PER_BLK);

  if (taps == 0 || input == 0)
    abort ();
}

void
qa_complex_dotprod_x86::tearDown ()
{
  free16Align (taps);
  free16Align (input);
  taps = 0;
  input = 0;
}


void 
qa_complex_dotprod_x86::zb ()	// "zero both"
{
  zero_floats (taps, MAX_BLKS * FLOATS_PER_BLK);
  zero_shorts (input, MAX_BLKS * SHORTS_PER_BLK);
}

// 
// t1 
//

void
qa_complex_dotprod_x86::t1_base (complex_dotprod_t complex_dotprod)
{
  float result[2];

  // cerr << "Testing dump_xmm_regs\n";
  // dump_xmm_regs ();

  // test basic cases, 1 block

  zb ();
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (0.0, 0.0, result, ERR_DELTA);

  // vary each input

  zb ();
  input[0] = 1;	taps[0] = 1.0; taps[1] = -1.0;
  complex_dotprod (input, taps, 1, result);
  //cerr << result[0] << " " << result[1] << "\n";
  assertcomplexEqual (1.0, -1.0, result, ERR_DELTA);
  
  zb ();
  input[1] = 2;	taps[2] = 1.0; taps[3] = -1.0;
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (2.0, -2.0, result, ERR_DELTA);

  zb ();
  input[2] = 3;	taps[4] = 1.0; taps[5] = -1.0;
  complex_dotprod (input, taps, 2, result);
  assertcomplexEqual (3.0, -3.0, result, ERR_DELTA);
  
  zb ();
  input[3] = 4;	taps[6] = 1.0; taps[7] = -1.0;
  complex_dotprod (input, taps, 2, result);
  assertcomplexEqual (4.0, -4.0, result, ERR_DELTA);

  // vary each tap

  zb ();
  input[0] = 1;	taps[0] = 0.5; taps[1] = -0.5;
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (0.5, -0.5, result, ERR_DELTA);
  
  zb ();
  input[0] = 1;	taps[0] = 2.0; taps[1] = -2.0;
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (2.0, -2.0, result, ERR_DELTA);
  
  zb ();
  input[0] = 1;	taps[0] = 3.0; taps[1] = -3.0;
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (3.0, -3.0, result, ERR_DELTA);
  
  zb ();
  input[0] = 1;	taps[0] = 4.0; taps[1] = -4.0;
  complex_dotprod (input, taps, 1, result);
  assertcomplexEqual (4.0, -4.0, result, ERR_DELTA);
}

// 
// t2 
//
void
qa_complex_dotprod_x86::t2_base (complex_dotprod_t complex_dotprod)
{
  float result[2];

  zb ();
  input[0] =  1;	taps[0] =  2.0;	taps[1] =  -2.0;
  input[1] =  3;	taps[2] =  5.0;	taps[3] =  -5.0;
  input[2] =  7;	taps[4] = 11.0;	taps[5] =  -11.0;
  input[3] = 13;	taps[6] = 17.0;	taps[7] =  -17.0;

  complex_dotprod (input, taps, 2, result);
  assertcomplexEqual (315.0, -315.0, result, ERR_DELTA);

  input[4] = 19;	taps[8] = 23.0;	taps[9] = -23.0;
  complex_dotprod (input, taps, 3, result);
  assertcomplexEqual (752.0, -752.0, result, ERR_DELTA);
  
}

//
// t3
//
void
qa_complex_dotprod_x86::t3_base (complex_dotprod_t complex_dotprod)
{
  srandom (0);	// we want reproducibility

  for (unsigned int i = 0; i < 10; i++){
    random_shorts (input, MAX_BLKS * SHORTS_PER_BLK);
    random_floats (taps, MAX_BLKS * FLOATS_PER_BLK);

    // we use a sloppy error margin because on the x86 architecture,
    // our reference implementation is using 80 bit floating point
    // arithmetic, while the SSE version is using 32 bit float point
    // arithmetic.

    float ref[2];
    ref_complex_dotprod (input, taps, MAX_BLKS, ref);
    float calc[2];
    complex_dotprod (input, taps, MAX_BLKS, calc);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref[0],
		    	calc[0],
			fabs (ref[0]) * 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref[1],
		    	calc[1],
			fabs (ref[1]) * 1e-4);
  }
}

void
qa_complex_dotprod_x86::t1_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t1_base (complex_dotprod_3dnowext);
}

void 
qa_complex_dotprod_x86::t2_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t2_base (complex_dotprod_3dnowext);
}

void 
qa_complex_dotprod_x86::t3_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t3_base (complex_dotprod_3dnowext);
}

void
qa_complex_dotprod_x86::t1_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t1_base (complex_dotprod_3dnow);
}

void 
qa_complex_dotprod_x86::t2_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t2_base (complex_dotprod_3dnow);
}

void 
qa_complex_dotprod_x86::t3_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t3_base (complex_dotprod_3dnow);
}

void 
qa_complex_dotprod_x86::t1_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t1_base (complex_dotprod_sse);
}

void 
qa_complex_dotprod_x86::t2_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t2_base (complex_dotprod_sse);
}

void 
qa_complex_dotprod_x86::t3_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t3_base (complex_dotprod_sse);
}

