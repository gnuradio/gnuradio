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
#include <stdlib.h>
#include <stdio.h>
#include <gnuradio/atsc/single_viterbi_impl.h>
#include "qa_atsci_single_viterbi.h"
#include <gnuradio/random.h>
#include <string.h>


static const int NTRIALS     =   50;
static const int MAXERRORS   =   10;
static const int NN          =  200;

static const int MAXDIBIT    = 3;

static gr::random rndm;

void
qa_atsci_single_viterbi::encode_block (unsigned char *out, unsigned char *in,
				      unsigned int n)
{
  for (unsigned int i = 0; i < n; i++) {
    out[i] = encoder.encode(in[i]);
  }
}


void
qa_atsci_single_viterbi::decode_block (unsigned char *out, unsigned char *in,
				      unsigned int n, float noise_factor)
{
  for (unsigned int i = 0; i < n; i++) {
    out[i] = decoder.decode((2*in[i]-7) + noise () * noise_factor);
  }
}

float
qa_atsci_single_viterbi::noise ()
{
  return 2.0 * (rndm.ran1() - 0.5);	// uniformly (-1, 1)
}

void
qa_atsci_single_viterbi::t0 ()
{
  static const int 			  blocklen = NN;
  unsigned char		  in[blocklen];
  unsigned char		  enc[blocklen];
  unsigned char		  out[blocklen];
  int			  decoder_errors = 0;
  int			  delay = decoder.delay ();
  int			  i;

  // printf ("  Delay is %d.\n", delay);

  srandom (27);		// reproducible sequence of "random" values

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
    decode_block (out, enc, blocklen, 1.0);

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
      printf ("  incorrect data\n");

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

void
qa_atsci_single_viterbi::t1 ()
{
  static const int 			  blocklen = NN;
  unsigned char		  in[blocklen];
  unsigned char		  enc[blocklen];
  unsigned char		  out[blocklen];
  int			  errlocs[NN];
  int			  errval;
  int			  errloc;
  int			  decoder_errors = 0;
  int			  delay = decoder.delay ();
  int			  i;

  // printf ("  Delay is %d.\n", delay);

  srandom (1);		// reproducible sequence of "random" values

  for (int nt = 0; nt < NTRIALS; nt++){

    // test up to the error correction capacity of the code
    for (int errors = 0; errors <= MAXERRORS; errors++){

      // load block with random data and encode

      for (i = 0; i < (blocklen-delay); i++)
	in[i] = random () & MAXDIBIT;
      for (     ; i < blocklen; i++)
	in[i] = 0;		/* To empty the delay buffers */

      encoder.reset ();
      encode_block (enc, in, blocklen);

      // Now generate 0 to N errors in the encoded symbols.
      //
      //  If we restrict ourselves to damaging the low-order bit,
      //  our decoder finds and fixes the vast majority of errors.
      //
      //  If we munge any or all of the three bits of the symbol,
      //  our decoder frequently gets the wrong data even with a single
      //  error.
      //
      //  Let's see what it can do with just the two low-order bits.
      //
      // ALSO:  Don't let any error be within 12 spots of another
      //  error.  This simulates the muxed behavior.

      memset (errlocs, 0, sizeof (errlocs));

      for (int j = 0; j < errors; j++){

	do {
	  // errval = random () & 3;  // FIXME:  1;   // FIXME: MAXSYM;
	  errval = random () & 1;  // FIXME:  1;   // FIXME: MAXSYM;
	} while (errval == 0);	// error value must be non-zero

	// Don't insert errors in the first delay slot, since we
	// don't have valid history to correct them.  Also, don't
	// insert burst errors (adjacent errors), or errors within 2,
	// since we can't reliably correct them.  Also we must not choose
	// the same location twice when inserting an error.

	do {
	  errloc = random () % NN;
	} while (errloc < delay || errlocs[errloc] != 0
		 || (errloc > 0 && errlocs[errloc-1] != 0)
		 || (errloc > 1 && errlocs[errloc-2] != 0)
		 || (errloc < (NN-1) && errlocs[errloc+1] != 0)
		 || (errloc < (NN-2) && errlocs[errloc+2] != 0));

	errlocs[errloc] = 1;

	enc[errloc] ^= errval;		// cause the error
      }

      // decode the errored block
      decoder.reset ();
      decode_block (out, enc, blocklen, 0.5);

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
	printf ("  %2d errors introduced, %scorrect data\n",
		 errors, differs? "in": "");

	// FIXME, should we be able to tell how many errs too?
        if (differs) {
	  //const int ERRTOL = 12;		/* Or relate to delay? */
	  int shouldfix = 1;
	  //int lasti = -ERRTOL;

	  printf (  "  Inserted errors:        ");
	  for (int erri = 0; erri < NN; erri++) {
	    if (errlocs[erri]) {
	      printf (" %d", erri);
	      // if (erri < lasti+ERRTOL)
	      //   shouldfix = 0;
	      //lasti = erri;
	    }
	  }
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
	  if (shouldfix)
	    printf ("\n    THIS IS A REAL PROBLEM.\n");
	  else
	    printf ("\n    BUT THAT'S OK since errors are too close.\n");
	  if (shouldfix)
            decoder_errors++;
	}
      }
    }
  }

  printf ("  Summary: %d decoder errors out of %d trials.\n",
	  decoder_errors, (MAXERRORS*NTRIALS));

  CPPUNIT_ASSERT (decoder_errors <= (MAXERRORS*NTRIALS) * .1);
}
