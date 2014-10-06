/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2005,2013 Free Software Foundation, Inc.
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
#include <stdlib.h>
#include <sys/time.h>

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <unistd.h>
#include <gnuradio/vco.h>
#include <gnuradio/fxpt_vco.h>
#include <string.h>

#define ITERATIONS 5000000
#define BLOCK_SIZE (10 * 1000)	// fits in cache

#define FREQ 5003.123
#define K 4.9999999
#define AMPLITUDE 2.444444444


static double
timeval_to_double(const struct timeval *tv)
{
  return (double) tv->tv_sec + (double) tv->tv_usec * 1e-6;
}


static void
benchmark(void test (float *x, const float *y), const char *implementation_name)
{
#ifdef HAVE_SYS_RESOURCE_H
  struct rusage	rusage_start;
  struct rusage	rusage_stop;
#else
  double clock_start;
  double clock_end;
#endif
  float output[BLOCK_SIZE];
  float input[BLOCK_SIZE];

  // touch memory
  memset(output, 0, BLOCK_SIZE*sizeof(float));
  for(int i = 0; i<BLOCK_SIZE; i++)
    input[i] = sin(double(i));

  // get starting CPU usage
#ifdef HAVE_SYS_RESOURCE_H
  if(getrusage (RUSAGE_SELF, &rusage_start) < 0) {
    perror("getrusage");
    exit(1);
  }
#else
  clock_start = (double)clock() * (1000000. / CLOCKS_PER_SEC);
#endif
  // do the actual work

  test(output, input);

  // get ending CPU usage

#ifdef HAVE_SYS_RESOURCE_H
  if(getrusage (RUSAGE_SELF, &rusage_stop) < 0) {
    perror("getrusage");
    exit(1);
  }

  // compute results

  double user =
    timeval_to_double(&rusage_stop.ru_utime)
    - timeval_to_double(&rusage_start.ru_utime);

  double sys =
    timeval_to_double(&rusage_stop.ru_stime)
    - timeval_to_double(&rusage_start.ru_stime);

  double total = user + sys;
#else
  clock_end = (double)clock() * (1000000. / CLOCKS_PER_SEC);
  double total = clock_end - clock_start;
#endif

  printf("%18s:  cpu: %6.3f  steps/sec: %10.3e\n",
         implementation_name, total, ITERATIONS / total);
}

// ----------------------------------------------------------------

void basic_vco(float *output, const float *input)
{
  double phase = 0;

  for(int j = 0; j < ITERATIONS/BLOCK_SIZE; j++) {
    for(int i = 0; i < BLOCK_SIZE; i++) {
      output[i] = cos(phase) * AMPLITUDE;
      phase += input[i] * K;

      while(phase > 2 * M_PI)
	phase -= 2 * M_PI;

      while(phase < -2 * M_PI)
	phase += 2 * M_PI;
    }
  }
}

void native_vco(float *output, const float *input)
{
  gr::blocks::vco<float,float> vco;

  for(int j = 0; j < ITERATIONS/BLOCK_SIZE; j++) {
    vco.cos(output, input, BLOCK_SIZE, K, AMPLITUDE);
  }
}

void fxpt_vco(float *output, const float *input)
{
  gr::blocks::fxpt_vco vco;

  for(int j = 0; j < ITERATIONS/BLOCK_SIZE; j++) {
    vco.cos(output, input, BLOCK_SIZE, K, AMPLITUDE);
  }
}

// ----------------------------------------------------------------

void nop_fct(float *x, const float *y)
{
}

void nop_loop(float *x, const float *y)
{
  for(int i = 0; i < ITERATIONS; i++) {
    nop_fct(x, y);
  }
}

int
main(int argc, char **argv)
{
  benchmark(nop_loop, "nop loop");
  benchmark(basic_vco, "basic vco");
  benchmark(native_vco, "native vco");
  benchmark(fxpt_vco, "fxpt vco");
}
