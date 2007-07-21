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
#include <qa_float_dotprod_x86.h>
#include <float_dotprod_x86.h>
#include <string.h>
#include <iostream>
#include <malloc16.h>
#include <sse_debug.h>
#include <cmath>
#include <gr_cpu.h>
#include <random.h>

using std::cerr;


#define	MAX_BLKS	10
#define	FLOATS_PER_BLK	 4

#define	ERR_DELTA	(1e-6)


static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = random () - RANDOM_MAX/2;
}

static void
zero_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = 0.0;
}

float 
ref_float_dotprod (const float *input,
		   const float *taps, unsigned n_4_float_blocks)
{
  float sum0 = 0;
  float sum1 = 0;
  float sum2 = 0;
  float sum3 = 0;

  do {

    sum0 += input[0] * taps[0];
    sum1 += input[1] * taps[1];
    sum2 += input[2] * taps[2];
    sum3 += input[3] * taps[3];

    input += 4;
    taps += 4;

  } while (--n_4_float_blocks != 0);


  return sum0 + sum1 + sum2 + sum3;
}

void 
qa_float_dotprod_x86::setUp ()
{
  taps = (float *) calloc16Align (MAX_BLKS, 
				  sizeof (float) * FLOATS_PER_BLK);

  input = (float *) calloc16Align (MAX_BLKS,
				   sizeof (float) * FLOATS_PER_BLK);

  if (taps == 0 || input == 0)
    abort ();
}

void
qa_float_dotprod_x86::tearDown ()
{
  free16Align (taps);
  free16Align (input);
  taps = 0;
  input = 0;
}


void 
qa_float_dotprod_x86::zb ()	// "zero both"
{
  zero_floats (taps, MAX_BLKS * FLOATS_PER_BLK);
  zero_floats (input, MAX_BLKS * FLOATS_PER_BLK);
}

// 
// t1 
//

void
qa_float_dotprod_x86::t1_base (float_dotprod_t float_dotprod)
{
  
  // cerr << "Testing dump_xmm_regs\n";
  // dump_xmm_regs ();

  // test basic cases, 1 block

  zb ();
  CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, float_dotprod (input, taps, 1), ERR_DELTA);

  // vary each input

  zb ();
  input[0] = 0.5;	taps[0] = 1.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (0.5, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[1] = 2.0;	taps[1] = 1.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (2.0, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[2] = 3.0;	taps[2] = 1.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (3.0, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[3] = 4.0;	taps[3] = 1.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (4.0, float_dotprod (input, taps, 1), ERR_DELTA);
  
  // vary each tap

  zb ();
  input[0] = 1.0;	taps[0] = 0.5;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (0.5, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 2.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (2.0, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 3.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (3.0, float_dotprod (input, taps, 1), ERR_DELTA);
  
  zb ();
  input[0] = 1.0;	taps[0] = 4.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (4.0, float_dotprod (input, taps, 1), ERR_DELTA);
}

// 
// t2 
//
void
qa_float_dotprod_x86::t2_base (float_dotprod_t float_dotprod)
{
  zb ();
  input[0] =  1.0;	taps[0] =  2.0;
  input[1] =  3.0;	taps[1] =  5.0;
  input[2] =  7.0;	taps[2] = 11.0;
  input[3] = 13.0;	taps[3] = 17.0;

  CPPUNIT_ASSERT_DOUBLES_EQUAL (315.0, float_dotprod (input, taps, 1), ERR_DELTA);

  input[4] = 19.0;	taps[4] = 23.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL (752.0, float_dotprod (input, taps, 2), ERR_DELTA);
  
}

//
// t3
//
void
qa_float_dotprod_x86::t3_base (float_dotprod_t float_dotprod)
{
  srandom (0);	// we want reproducibility

  for (unsigned int i = 0; i < 10; i++){
    random_floats (input, MAX_BLKS * FLOATS_PER_BLK);
    random_floats (taps, MAX_BLKS * FLOATS_PER_BLK);

    // we use a sloppy error margin because on the x86 architecture,
    // our reference implementation is using 80 bit floating point
    // arithmetic, while the SSE version is using 32 bit float point
    // arithmetic.

    float ref = ref_float_dotprod (input, taps, MAX_BLKS);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (ref,
			float_dotprod (input, taps, MAX_BLKS),
			fabs (ref) * 1e-4);
  }
}

void
qa_float_dotprod_x86::t1_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t1_base (float_dotprod_3dnow);
}

void 
qa_float_dotprod_x86::t2_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t2_base (float_dotprod_3dnow);
}

void 
qa_float_dotprod_x86::t3_3dnow ()
{
  if (!gr_cpu::has_3dnow ()){
    cerr << "No 3DNow! support; not tested\n";
  }
  else
    t3_base (float_dotprod_3dnow);
}

void 
qa_float_dotprod_x86::t1_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t1_base (float_dotprod_sse);
}

void 
qa_float_dotprod_x86::t2_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t2_base (float_dotprod_sse);
}

void 
qa_float_dotprod_x86::t3_sse ()
{
  if (!gr_cpu::has_sse ()){
    cerr << "No SSE support; not tested\n";
  }
  else
    t3_base (float_dotprod_sse);
}
