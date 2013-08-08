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
#include <stdio.h>
#include <gnuradio/atsc/fake_single_viterbi_impl.h>
#include "qa_atsci_fake_single_viterbi.h"
#include <gnuradio/random.h>
#include <string.h>


static const int NTRIALS     =   50;
static const int MAXERRORS   =   10;
static const int NN          =  200;

static const int MAXDIBIT    = 3;

void
qa_atsci_fake_single_viterbi::encode_block (unsigned char *out, unsigned char *in,
				      unsigned int n)
{
  for (unsigned int i = 0; i < n; i++) {
    out[i] = encoder.encode(in[i]);
  }
}


void
qa_atsci_fake_single_viterbi::decode_block (unsigned char *out, unsigned char *in,
				      unsigned int n)
{
  for (unsigned int i = 0; i < n; i++) {
    out[i] = decoder.decode ((2*in[i]-7) + noise ());
  }
}

float
qa_atsci_fake_single_viterbi::noise ()
{
#if 1
  return 2.0 * ((float) random () / RANDOM_MAX - 0.5);;
#else
  return 0;
#endif
}

void
qa_atsci_fake_single_viterbi::t0 ()
{
  static const int 			  blocklen = NN;
  unsigned char		  in[blocklen];
  unsigned char		  enc[blocklen];
  unsigned char		  out[blocklen];
  int			  decoder_errors = 0;
  int			  delay = decoder.delay ();
  int			  i;

  // printf ("  Delay is %d.\n", delay);

  srandom (27);		// reproducable sequence of "random" values

  for (int nt = 0; nt < NTRIALS; nt++){

    // load block with random data and encode

    for (i = 0; i < (blocklen-delay); i++)
      in[i] = random () & MAXDIBIT;
    for (     ; i < blocklen; i++)
      in[i] = 0;		/* To empty the delay buffers */

    encoder.reset ();
    encode_block (enc, in, blocklen);

    decoder.reset ();

    // decode the block
    decode_block (out, enc, blocklen);

    // int offset = delay/4;
    int offset = 2;
    bool differs = (memcmp (in+offset,
			    out+delay+offset, blocklen-(delay+offset)));

    // initial values after reset are 0
    for (i = 0; i < delay; i++){
      if (out[i] != 0)
	printf ("  initial output at %i is %X, not 0\n",
		i, out[i]);
    }

    if (differs){
      printf ("  incorrect data, trial #%d\n", nt);

      printf ("\n  Erroneous result dibits:");
      for (int erri = 0; erri < (NN-delay); erri++) {
	if (in[erri] != out[erri+delay])
	  printf (" %d", erri);
      }
      printf ("\n  In:  ");
      for (int erri = 0; erri < (NN-delay); erri++) {
	printf (" %d", in[erri]);
      }
      printf ("\n  Out: ");
      for (int erri = 0; erri < (NN-delay); erri++) {
	printf (" %d", out[erri+delay]);
      }
      printf ("\n  Errs:");
      for (int erri = 0; erri < (NN-delay); erri++) {
	printf (" %c", (in[erri] != out[erri+delay])? '*': ' ');
      }
      printf ("\n    THIS IS A REAL PROBLEM.\n");
      decoder_errors++;
    }
  }

  printf ("  Summary: %d decoder errors out of %d trials.\n",
	  decoder_errors, NTRIALS);

  CPPUNIT_ASSERT (decoder_errors == 0);
}

