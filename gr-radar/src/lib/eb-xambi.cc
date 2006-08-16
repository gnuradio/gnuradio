/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <boost/scoped_array.hpp>
#include <gr_complex.h>
#include <gr_fxpt_nco.h>
#include "time_series.h"


gr_complex
complex_conj_dotprod(const gr_complex *a, const gr_complex *b, unsigned n)
{
  gr_complex acc = 0;
  for (unsigned i = 0; i < n; i++)
    acc += a[i] * conj(b[i]);

  return acc;
}

/*!
 * \brief frequency translate src by normalized_freq
 *
 * \param dst			destination
 * \param src			source
 * \param n			length of src and dst in samples
 * \param normalized_freq	[-1/2, +1/2]
 */
void
freq_xlate(gr_complex *dst, const gr_complex *src, unsigned n, float normalized_freq)
{
  gr_fxpt_nco	nco;
  nco.set_freq(2 * M_PI * normalized_freq);

  for (unsigned int i = 0; i < n; i++){
    gr_complex	phasor(nco.cos(), nco.sin());
    dst[i] = src[i] * phasor;
    nco.step();
  }
}

inline void
write_float(FILE *output, float x)
{
  fwrite(&x, sizeof(x), 1, output);
}


// write 8-float header
static void
write_header (FILE *output, float ndoppler_bins, float min_doppler, float max_doppler)
{
  write_float(output, ndoppler_bins);
  write_float(output, min_doppler);
  write_float(output, max_doppler);
  write_float(output, 0);
  write_float(output, 0);
  write_float(output, 0);
  write_float(output, 0);
  write_float(output, 0);
}


void
main_loop(FILE *output, time_series &ref_ts, time_series &scat0_ts,
	  unsigned nranges, unsigned correlation_window_size,
	  float min_doppler, float max_doppler, int ndoppler_bins)
{
  fprintf(stderr, "ndoppler_bins = %10d\n", ndoppler_bins);
  fprintf(stderr, "min_doppler   = %10f\n", min_doppler);
  fprintf(stderr, "max_doppler   = %10f\n", max_doppler);

  // float scale_factor = 1.0/correlation_window_size;	// FIXME, not sure this is right
  float scale_factor = 1.0;				// FIXME, not sure this is right

  boost::scoped_array<gr_complex>  shifted(new gr_complex[correlation_window_size]);

  // gr_complex shifted[correlation_window_size];		// doppler shifted reference

  float doppler_incr = 0;
  if (ndoppler_bins == 1){
    min_doppler = 0;
    max_doppler = 0;
  }
  else
    doppler_incr = (max_doppler - min_doppler) / (ndoppler_bins - 1);

  write_header(output, ndoppler_bins, min_doppler, max_doppler);

  unsigned long long ro = 0;	// reference offset
  unsigned long long so = 0;	// scatter offset

  for (unsigned int n = 0; n < nranges; n++){
    if (0){
      fprintf(stdout, "n =  %6d\n", n);
      fprintf(stdout, "ro = %6lld\n", ro);
      fprintf(stdout, "so = %6lld\n", so);
    }
    const gr_complex *ref = (const gr_complex *) ref_ts.seek(ro, correlation_window_size);
    const gr_complex *scat0 = (const gr_complex *) scat0_ts.seek(so, correlation_window_size);
    if (ref == 0 || scat0 == 0)
      return;

    for (int nd = 0; nd < ndoppler_bins; nd++){
      float fdop = min_doppler + doppler_incr * nd;
      //fprintf(stderr, "fdop = %10g\n", fdop);
      freq_xlate(&shifted[0], ref, correlation_window_size, fdop);	// generated doppler shifted reference

      gr_complex ccor = complex_conj_dotprod(&shifted[0], scat0, correlation_window_size);
      float out = norm(ccor) * scale_factor;

      // fprintf(output, "%12g\n", out);
      write_float(output, out);
    }

    so += 1;
  }
}

static void
usage(const char *argv0)
{
  const char *progname;
  const char *t = std::strrchr(argv0, '/');
  if (t != 0)
    progname = t + 1;
  else
    progname = argv0;
    
  fprintf(stderr, "usage: %s [options] ref_file scatter_file\n", progname);
  fprintf(stderr, "    -o OUTPUT_FILENAME [default=eb-xambi.out]\n");
  fprintf(stderr, "    -m MIN_RANGE [default=0]\n");
  fprintf(stderr, "    -M MAX_RANGE [default=300]\n");
  fprintf(stderr, "    -w CORRELATION_WINDOW_SIZE [default=2500]\n");
  fprintf(stderr, "    -s NSAMPLES_TO_SKIP [default=0]\n");
  fprintf(stderr, "    -d max_doppler (normalized: [0, +1/2)) [default=.0012]\n");
  fprintf(stderr, "    -n ndoppler_bins [default=31]\n");
}

int
main(int argc, char **argv)
{
  int	ch;
  int min_range =    0;
  int max_range =  300;
  const char *ref_filename = 0;
  const char *scatter_filename = 0;
  const char *output_filename = "eb-xambi.out";
  unsigned int correlation_window_size = 2500;
  long long int nsamples_to_skip = 0;
  double max_doppler = 0.0012;
  int ndoppler_bins = 31;


  while ((ch = getopt(argc, argv, "m:M:ho:w:s:d:n:")) != -1){
    switch (ch){
    case 'm':
      min_range = strtol(optarg, 0, 0);
      break;

    case 'M':
      max_range = strtol(optarg, 0, 0);
      break;

    case 'w':
      correlation_window_size = strtol(optarg, 0, 0);
      if (correlation_window_size <= 1){
	usage(argv[0]);
	fprintf(stderr, "    correlation_window_size must be >= 1\n");
	exit(1);
      }
      break;

    case 'o':
      output_filename = optarg;
      break;
      
    case 's':
      nsamples_to_skip = (long long) strtod(optarg, 0);
      if (nsamples_to_skip < 0){
	usage(argv[0]);
	fprintf(stderr, "    nsamples_to_skip must be >= 0\n");
	exit(1);
      }
      break;

    case 'd':
      max_doppler = strtod(optarg, 0);
      if (max_doppler < 0 || max_doppler >= 0.5){
	usage(argv[0]);
	fprintf(stderr, "    max_doppler must be in [0, 0.5)\n");
	exit(1);
      }
      break;

    case 'n':
      ndoppler_bins = strtol(optarg, 0, 0);
      if (ndoppler_bins < 1){
	usage(argv[0]);
	fprintf(stderr, "    ndoppler_bins must >= 1\n");
	exit(1);
      }
      break;

    case '?':
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  } // while getopt

  if (argc - optind != 2){
    usage(argv[0]);
    exit(1);
  }

  if (max_range < min_range){
    usage(argv[0]);
    fprintf(stderr, "    max_range must be >= min_range\n");
    exit(1);
  }
  unsigned int nranges = max_range - min_range + 1;

  ref_filename = argv[optind++];
  scatter_filename = argv[optind++];

  FILE *output = fopen(output_filename, "wb");
  if (output == 0){
    perror(output_filename);
    exit(1);
  }

  unsigned long long ref_starting_offset = 0;
  unsigned long long scatter_starting_offset = 0;

  if (min_range < 0){
    ref_starting_offset = -min_range;
    scatter_starting_offset = 0;
  }
  else {
    ref_starting_offset = 0;
    scatter_starting_offset = min_range;
  }

  ref_starting_offset += nsamples_to_skip;
  scatter_starting_offset += nsamples_to_skip;

  try {
    time_series ref(sizeof(gr_complex), ref_filename, ref_starting_offset, 0);
    time_series scat0(sizeof(gr_complex), scatter_filename, scatter_starting_offset, 0);

    main_loop(output, ref, scat0, nranges, correlation_window_size,
	      -max_doppler, max_doppler, ndoppler_bins);
  }
  catch (std::string &s){
    std::cerr << s << std::endl;
    exit(1);
  }

  return 0;
}

