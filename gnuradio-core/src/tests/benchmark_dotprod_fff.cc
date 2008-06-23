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
#include "config.h"
#endif
#include <stdio.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <unistd.h>
#include <gr_fir_util.h>
#include <gr_fir_fff.h>
#include <random.h>

#define TOTAL_TEST_SIZE	       (40 * 1000 * 1000L)
#define	NTAPS		                     256
#define	BLOCK_SIZE	             (50 * 1000)	/* fits in cache */

#if ((TOTAL_TEST_SIZE % BLOCK_SIZE) != 0)
#error "TOTAL_TEST_SIZE % BLOCK_SIZE must equal 0"
#endif

typedef gr_fir_fff* (*fir_maker_t)(const std::vector<float> &taps);
typedef gr_fir_fff  filter_t;


static double
timeval_to_double (const struct timeval *tv)
{
  return (double) tv->tv_sec + (double) tv->tv_usec * 1e-6;
}


static void
benchmark (fir_maker_t filter_maker, const char *implementation_name)
{
  int	i;
  float coeffs[NTAPS];
  float input[BLOCK_SIZE + NTAPS];
  long	n;
  float	result;
#ifdef HAVE_SYS_RESOURCE_H
  struct rusage	rusage_start;
  struct rusage	rusage_stop;
#else
  double clock_start;
  double clock_end;
#endif
  // setup coefficients and input data

  for (i = 0; i < NTAPS; i++)
    coeffs[i] = random() - RANDOM_MAX/2;

  for (i = 0; i < BLOCK_SIZE + NTAPS; i++)
    input[i] = random() - RANDOM_MAX/2;

  std::vector<float> taps (&coeffs[0], &coeffs[NTAPS]);
  filter_t *f = filter_maker (taps);

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

  for (n = 0; n < TOTAL_TEST_SIZE; n += BLOCK_SIZE){
    int	j;
    for (j = 0; j < BLOCK_SIZE; j++){
      result = f->filter (&input[j]);
    }
  }

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
  double total = clock_end -clock_start;
#endif
  double macs = NTAPS * (double) TOTAL_TEST_SIZE;

  printf ("%10s: taps: %4d  input: %4g  cpu: %6.3f  taps/sec: %10.4g  \n",
	  implementation_name, NTAPS, (double) TOTAL_TEST_SIZE, total, macs / total);

  delete f;
}

static void
do_all ()
{
  std::vector<gr_fir_fff_info>		info;
  gr_fir_util::get_gr_fir_fff_info (&info);	// get all known FFF implementations

  for (std::vector<gr_fir_fff_info>::iterator p = info.begin ();
       p != info.end ();
       ++p){

    benchmark (p->create, p->name);
  }
}

int
main (int argc, char **argv)
{
  do_all ();
  return 0;
}
