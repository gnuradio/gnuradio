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
#include <qa_ccomplex_dotprod_x86.h>
#include <ccomplex_dotprod_x86.h>
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

void
ref_ccomplex_dotprod (const float *input,
		      const float *taps, unsigned n_2_ccomplex_blocks,
		      float *result)
{
  float sum0[2] = {0,0};
  float sum1[2] = {0,0};

  do {

    sum0[0] += input[0] * taps[0] - input[1] * taps[1];
    sum0[1] += input[0] * taps[1] + input[1] * taps[0];
    sum1[0] += input[2] * taps[2] - input[3] * taps[3];
    sum1[1] += input[2] * taps[3] + input[3] * taps[2];

    input += 4;
    taps += 4;

  } while (--n_2_ccomplex_blocks != 0);


  result[0] = sum0[0] + sum1[0];
  result[1] = sum0[1] + sum1[1];
}

void 
qa_ccomplex_dotprod_x86::setUp ()
{
  taps = (float *) calloc16Align (MAX_BLKS, 
				  sizeof (float) * FLOATS_PER_BLK);

  input = (float *) calloc16Align (MAX_BLKS,
				   sizeof (float) * FLOATS_PER_BLK);

  if (taps == 0 || input == 0)
    abort ();
}

void
qa_ccomplex_dotprod_x86::tearDown ()
{
  free16Align (taps);
  free16Align (input);
  taps = 0;
  input = 0;
}


void 
qa_ccomplex_dotprod_x86::zb ()	// "zero both"
{
  zero_floats (taps, MAX_BLKS * FLOATS_PER_BLK);
  zero_floats (input, MAX_BLKS * FLOATS_PER_BLK);
}

// 
// t1 
//

void
qa_ccomplex_dotprod_x86::t1_base (ccomplex_dotprod_t ccomplex_dotprod)
{
  float result[2];

  // cerr << "Testing dump_xmm_regs\n";
  // dump_xmm_regs ();

  // test basic cases, 1 block

  zb ();
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (0.0, 0.0, result, ERR_DELTA);

  // vary each input

  zb ();
  input[0] = 1.0;	taps[0] = 1.0; taps[1] = -1.0;
  ccomplex_dotprod (input, taps, 1, result);
  //cerr << result[0] << " " << result[1] << "\n";
  assertcomplexEqual (1.0, -1.0, result, ERR_DELTA);
  
  zb ();
  input[1] = 2.0;	taps[0] = 1.0; taps[1] = -1.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (2.0, 2.0, result, ERR_DELTA);

  zb ();
  input[2] = 3.0;	taps[2] = 1.0; taps[3] = -1.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (3.0, -3.0, result, ERR_DELTA);
  
  zb ();
  input[3] = 4.0;	taps[2] = 1.0; taps[3] = -1.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (4.0, 4.0, result, ERR_DELTA);

  // vary each tap

  zb ();
  input[0] = 1.0;	taps[0] = 0.5; taps[1] = -0.5;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (0.5, -0.5, result, ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 2.0; taps[1] = -2.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (2.0, -2.0, result, ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 3.0; taps[1] = -3.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (3.0, -3.0, result, ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 4.0; taps[1] = -4.0;
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (4.0, -4.0, result, ERR_DELTA);
}

// 
// t2 
//
void
qa_ccomplex_dotprod_x86::t2_base (ccomplex_dotprod_t ccomplex_dotprod)
{
  float result[2];

  zb ();
  input[0] =  1.0; input[1] =  3.0;	taps[0] =  5.0;	taps[1] =  -2.0;

  //1*5-3*-2 =11, 1*-2+3*5=13
 
  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (11.0, 13.0, result, ERR_DELTA);

  //7*5-13*-5 =100, 7*-5+13*5=30

  input[2] =  7.0; input[3] = 13.0;	taps[2] =  5.0;	taps[3] =  -5.0;

  ccomplex_dotprod (input, taps, 1, result);
  assertcomplexEqual (111.0, 43.0, result, ERR_DELTA);

  input[4] = 19; input[5] = -19;	taps[4] = 23.0;	taps[5] = -23.0;

  //19*23--19*-23 =0, 19*-23+-19*23=-874

  ccomplex_dotprod (input, taps, 2, result);
  assertcomplexEqual (111.0, -831.0, result, ERR_DELTA);
  
}

//
// t3
//
void
qa_ccomplex_dotprod_x86::t3_base (ccomplex_dotprod_t ccomplex_dotprod)
{
  srandom (0);	// we want reproducibility

  for (unsigned int i = 0; i < 10; i++){
    random_floats (input, MAX_BLKS * FLOATS_PER_BLK);
    random_floats (taps, MAX_BLKS * FLOATS_PER_BLK);

    // we use a sloppy error margin because on the x86 architecture,
    // our reference implementation is using 80 bit floating point
    // arithmetic, while the SSE version is using 32 bit float point
    // arithmetic.

    float ref[2];
    ref_ccomplex_dotprod (input, taps, MAX_BLKS, ref);
    float calc[2];
    ccomplex_dotprod (input, taps, MAX_BLKS, calc);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref[0],
		    	calc[0],
			fabs (ref[0]) * 1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref[1],
		    	calc[1],
			fabs (ref[1]) * 1e-4);
  }
}

void
qa_ccomplex_dotprod_x86::t1_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t1_base (ccomplex_dotprod_3dnowext);
}

void 
qa_ccomplex_dotprod_x86::t2_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t2_base (ccomplex_dotprod_3dnowext);
}

void 
qa_ccomplex_dotprod_x86::t3_3dnowext ()
{
  if (!gr_cpu::has_3dnowext ()){
    cerr << "No 3DNow!Ext support; not tested\n";
  }
  else
    t3_base (ccomplex_dotprod_3dnowext);
}

void
qa_ccomplex_dotprod_x86::t1_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t1_base (ccomplex_dotprod_3dnow);
}

void 
qa_ccomplex_dotprod_x86::t2_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t2_base (ccomplex_dotprod_3dnow);
}

void 
qa_ccomplex_dotprod_x86::t3_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t3_base (ccomplex_dotprod_3dnow);
}

void 
qa_ccomplex_dotprod_x86::t1_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t1_base (ccomplex_dotprod_sse);
}

void 
qa_ccomplex_dotprod_x86::t2_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t2_base (ccomplex_dotprod_sse);
}

void 
qa_ccomplex_dotprod_x86::t3_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t3_base (ccomplex_dotprod_sse);
}

