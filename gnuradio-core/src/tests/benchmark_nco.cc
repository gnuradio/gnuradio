/* -*- c++ -*- */
/*
 * Copyright 2002,2004 Free Software Foundation, Inc.
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
#include "config.h"
#endif
#include <stdio.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <unistd.h>
#include <gr_nco.h>
#include <gr_fxpt_nco.h>
#include <string.h>

#define ITERATIONS	20000000
#define BLOCK_SIZE	(10 * 1000)	// fits in cache

#define FREQ	5003.123

static double
timeval_to_double (const struct timeval *tv)
{
  return (double) tv->tv_sec + (double) tv->tv_usec * 1e-6;
}


static void
benchmark (void test (float *x, float *y), const char *implementation_name)
{
#ifdef HAVE_SYS_RESOURCE_H
  struct rusage	rusage_start;
  struct rusage	rusage_stop;
#else
  double clock_start;
  double clock_end;
#endif
  float output[2*BLOCK_SIZE];
  float *x = &output[0], *y = &output[BLOCK_SIZE];

  // touch memory
  memset(output, 0, 2*BLOCK_SIZE*sizeof(float));

  // get starting CPU usage
#ifdef HAVE_SYS_RESOURCE_H
  if (getrusage (RUSAGE_SELF, &rusage_start) < 0){
    perror ("getrusage");
    exit (1);
  }
#else
  clock_start = (double) clock() * (1000000. / CLOCKS_PER_SEC);
#endif
  // do the actual work

  test (x, y);

  // get ending CPU usage

#ifdef HAVE_SYS_RESOURCE_H
  if (getrusage (RUSAGE_SELF, &rusage_stop) < 0){
    perror ("getrusage");
    exit (1);
  }

  // compute results

  double user =
    timeval_to_double (&rusage_stop.ru_utime)
    - timeval_to_double (&rusage_start.ru_utime);

  double sys =
    timeval_to_double (&rusage_stop.ru_stime)
    - timeval_to_double (&rusage_start.ru_stime);

  double total = user + sys;
#else
  clock_end = (double) clock () * (1000000. / CLOCKS_PER_SEC);
  double total = clock_end - clock_start;
#endif

  printf ("%18s:  cpu: %6.3f  steps/sec: %10.3e\n",
	  implementation_name, total, ITERATIONS / total);
}

// ----------------------------------------------------------------
// Don't compare the _vec with other functions since memory store's
// are involved.

void basic_sincos_vec (float *x, float *y)
{
  gr_nco<float,float>	nco;

  nco.set_freq (2 * M_PI / FREQ);

  for (int i = 0; i < ITERATIONS/BLOCK_SIZE; i++){
    for (int j = 0; j < BLOCK_SIZE; j++){
      nco.sincos (&x[2*j+1], &x[2*j]);
      nco.step ();
    }
  }
}

void native_sincos_vec (float *x, float *y)
{
  gr_nco<float,float>	nco;

  nco.set_freq (2 * M_PI / FREQ);
 
  for (int i = 0; i < ITERATIONS/BLOCK_SIZE; i++){
    nco.sincos ((gr_complex*)x, BLOCK_SIZE);
  }
}

void fxpt_sincos_vec (float *x, float *y)
{
  gr_fxpt_nco	nco;
  
  nco.set_freq (2 * M_PI / FREQ);
  
  for (int i = 0; i < ITERATIONS/BLOCK_SIZE; i++){
    nco.sincos ((gr_complex*)x, BLOCK_SIZE);
  }
}

// ----------------------------------------------------------------

void native_sincos (float *x, float *y)
{
  gr_nco<float,float>	nco;

  nco.set_freq (2 * M_PI / FREQ);

  for (int i = 0; i < ITERATIONS; i++){
    nco.sincos (x, y);
    nco.step ();
  }
}

void fxpt_sincos (float *x, float *y)
{
  gr_fxpt_nco	nco;

  nco.set_freq (2 * M_PI / FREQ);

  for (int i = 0; i < ITERATIONS; i++){
    nco.sincos (x, y);
    nco.step ();
  }
}

// ----------------------------------------------------------------

void native_sin (float *x, float *y)
{
  gr_nco<float,float>	nco;

  nco.set_freq (2 * M_PI / FREQ);

  for (int i = 0; i < ITERATIONS; i++){
    *x = nco.sin ();
    nco.step ();
  }
}

void fxpt_sin (float *x, float *y)
{
  gr_fxpt_nco	nco;

  nco.set_freq (2 * M_PI / FREQ);

  for (int i = 0; i < ITERATIONS; i++){
    *x = nco.sin ();
    nco.step ();
  }
}

// ----------------------------------------------------------------

void nop_fct (float *x, float *y)
{
}

void nop_loop (float *x, float *y)
{
  for (int i = 0; i < ITERATIONS; i++){
    nop_fct (x, y);
  }
}

int
main (int argc, char **argv)
{
  benchmark (nop_loop, "nop loop");
  benchmark (native_sin, "native sine");
  benchmark (fxpt_sin, "fxpt sine");
  benchmark (native_sincos, "native sin/cos");
  benchmark (fxpt_sincos, "fxpt sin/cos");
  benchmark (basic_sincos_vec, "basic sin/cos vec");
  benchmark (native_sincos_vec, "native sin/cos vec");
  benchmark (fxpt_sincos_vec, "fxpt sin/cos vec");
}
