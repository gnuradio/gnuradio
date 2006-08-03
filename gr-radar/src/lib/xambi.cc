/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#include <gr_complex.h>
#include <gri_fft.h>
#include "time_series.h"


using namespace std;

/*!
 * \file xambi.cc driver for computation of cross ambiguity
 *
 * Based on ideas liberally lifted from a version of xambi.cc 
 * obtained from John Sahr, that identified these people as authors:
 *   John Sahr            jdsahr@u.washington.edu
 *   Frank Lind           flind@haystack.mit.edu
 *   Chucai "Cliff" Zhou
 *   Melissa Meyer        mgmeyer@ee.washington.edu
 *
 * Extensively revised since then.
 */

static int default_decimation = 40;
static int default_fftsize    = 256;
static int default_naverages   = 1000000; // infinite

/// usage for the xambi program

static void usage(char *argv0)
{
  cerr << "usage:  xambi [opts] scatterfile"                << endl;
  cerr << "  where [opts] are"                              << endl;
  cerr << "  -x reffilename                  : Required"    << endl;
  cerr << "  -o outputfilename[=xambi.out]   : create or overwrite" << endl;
  cerr << "  -a averages[=100000]            : restart accumulation; 100000 = infinite" << endl;
  cerr << "  -d decimationfactor[=40]"                      << endl;
  cerr << "  -f fftsize[=256]"                              << endl;
  cerr << "  -r range[=0]                    : first range" << endl;
  cerr << "  -n nranges[=1]                  : range count" << endl;
  cerr << "  -S start[=0]                    : starting offset in file" << endl;
  cerr << "  -N nsamples[=inf]               : # of samples to process" << endl;
  cerr << "  -v                              : increment verbosity" << endl;
  cerr << "  -h                              : be helpful" << endl;
  cerr << endl;
  cerr << "The reffile and scatterfile are native-endian binary complex<float>" << endl;
  cerr << "and must be sampled at the same rate." << endl;
 
  exit(0);
}

gr_complex
complex_dot_product(const gr_complex *xx, const gr_complex *yy, int nterms)
{
  gr_complex sum(0);

  for (int i = 0; i < nterms; i++)
    sum += xx[i] * yy[i];		// FIXME?   conj(yy[i])

  return sum;
}


/// the main driver

int 
main(int argc, char *argv[])
{
  char *ref_fname = 0;    //< holds name of reference signal source
  char *out_fname = 0;    //< holds name of processed output
  int decimation  = default_decimation;
  int range       = 0;    //< first range of range block
  int nranges     = 1;    //< number of ranges of range block
  int fftsize     = default_fftsize;
  int naverages   = default_naverages;
  int verbosity   = 0;    
  int blocksize   = 0;
  int offset      = 0;
  unsigned long long starting_file_offset = 0;
  unsigned long long nsamples_to_process = (unsigned long long) -1;

  int f;

  const gr_complex     *x,  *y;
  const gr_complex     *xx, *yy;

  int c;         // used to process the command line
  int r;         // an index to count over ranges
  int i;         // an index to count through the time series
  int a;

  while((c = getopt(argc,argv,"a:o:x:y:r:d:f:n:hvS:N:")) != EOF) {
    switch(c) {
    case 'x': ref_fname  = optarg;            break;
    case 'o': out_fname  = optarg;            break;
    case 'a': naverages   = atoi(optarg);     break;
    case 'd': decimation = atoi(optarg);      break;
    case 'r': range      = atoi(optarg);      break;
    case 'n': nranges    = atoi(optarg);      break;
    case 'f': fftsize    = atoi(optarg);      break;
    case 'S': starting_file_offset = strtoll(optarg, 0, 0);  break;
    case 'N': nsamples_to_process = strtoll(optarg, 0, 0);   break;
    case 'v': verbosity++;                    break;
    case 'h': usage(argv[0]);                 break;
    default:  usage(argv[0]);                 break;
    }
  }

  // Wrapper for FFTW 1d forward FFT.  N.B. output is not scaled by 1/fftsize
  gri_fft_complex	fft(fftsize, true); 
  gr_complex 	 	*fft_input = fft.get_inbuf();
  gr_complex		*fft_output = fft.get_outbuf();

  if(range < 0) {
    cerr << "you specified -r " << range << "; must be non-negative (exit)" << endl;
    exit(1);
  }

  if(nranges < 1) {
    cerr << "you specified -n " << nranges << "; must be positive (exit)" << endl;
    exit(1);
  }

  if(decimation < 1) {
    cerr << "you specified -d " << decimation << "; must be positive (exit)" << endl;
    exit(1);
  }

  if(naverages < 1) {
    cerr << "you specified -a " << naverages << "; must be positive (exit)" << endl;
    exit(1);
  }

  if(ref_fname == 0) {
    cerr << "you must specify a reference signal with the -x option" << endl;
    usage(argv[0]);
  }

  if (optind >= argc) {
    cerr << "you must specify a scattering signal after all other options" << endl;
    usage(argv[0]);
  }

  time_series   X(sizeof(gr_complex), ref_fname,
		  starting_file_offset, nsamples_to_process);

  time_series   Y(sizeof(gr_complex), argv[optind],
		  starting_file_offset, nsamples_to_process); // add more for interferometry ...

  float  psd[fftsize*nranges];

  if(out_fname == 0) {
    char fname[200];
    snprintf(fname, sizeof(fname), "%s.out", "xambi");
    out_fname = fname;
  }

  ofstream Z(out_fname);

  blocksize = fftsize*decimation + nranges;
  offset    = 0;
  a         = 0;

  // the fftsize is squared because we're using norm, not abs, 
  // when computing the psd
  float scale_factor = 1.0 / (fftsize * fftsize);

  for(i = 0; i < nranges*fftsize; i++)
    psd[i] = 0.0;


  while(1){ 		 // loop over data until exhausted.
    if(verbosity > 1) { 
      cerr << " " << a;  // write out the number of completed averages
      cerr.flush();
    }

    x = (const gr_complex *) X.seek(offset,         blocksize);
    y = (const gr_complex *) Y.seek(offset + range, blocksize);

    if(!x || !y)	  // ran out of data; stop integrating
      break;

    for(r = 0; r < nranges; r++) {  // For Each Range ...
      xx = x;
      yy = y + r;

      for(f = 0; f < fftsize; f++) {  // and for each Doppler bin ...

	// cross correlate and do a boxcar decimation

	fft_input[f] = complex_dot_product(xx, yy, decimation);
	xx += decimation;
	yy += decimation;
      }

      fft.execute();	// input: fft_input; output: fft_output

      for(f = 0; f < fftsize; f++) {
	psd[r*fftsize + f] += norm(fft_output[f]);
      }
    }	// end range

    a++;
    offset += fftsize * decimation; 

    if(a >= naverages) {
      if(verbosity > 0)
	cerr << " dumping " << endl;
      
      for(i = 0; i < nranges*fftsize; i++)	// normalize
	psd[i] *= scale_factor;

      Z.write((const char *) psd, nranges*fftsize*sizeof(float)); 
 
      for(i = 0; i < nranges*fftsize; i++)
	psd[i] = 0.0;
      
      a = 0;
    }
  }

  if(verbosity > 1)
    printf("\n");

  if(a > 0) {
    for(i = 0; i < nranges*fftsize; i++)	// normalize
      psd[i] *= scale_factor;

    Z.write((const char *) psd, nranges*fftsize*sizeof(float));
  }

  return 0;
}

