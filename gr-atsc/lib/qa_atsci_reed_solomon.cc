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

#include <gnuradio/atsc/reed_solomon_impl.h>
#include <boost/test/unit_test.hpp>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static const int NROOTS      =   20;
static const int NTRIALS     =  100;
static const int NN	     = ATSC_MPEG_RS_ENCODED_LENGTH;


BOOST_AUTO_TEST_CASE(t0_reed_solomon)
{
  atsci_reed_solomon rs;

  atsc_mpeg_packet_no_sync	in;
  atsc_mpeg_packet_rs_encoded  	enc;
  atsc_mpeg_packet_no_sync	out;
  int			  derrors;
  int			  errlocs[NN];
  int			  errval;
  int			  errloc;
  int			  decoder_errors = 0;

  for (int nt = 0; nt < NTRIALS; nt++){

    // test up to the error correction capacity of the code
    for (int errors = 0; errors <= NROOTS*2; errors++){

      // load block with random data and encode

      for (int i = 0; i < ATSC_MPEG_DATA_LENGTH; i++)
	in.data[i] = random () & 0xff;

      rs.encode (enc, in);

      memset (errlocs, 0, sizeof (errlocs));

      for (int i = 0; i < errors; i++){

	do {
	  errval = random () & 0xff;
	} while (errval == 0);	// error value must be non-zero

	do {
	  errloc = random () % NN;
	} while (errlocs[errloc] != 0);		// must not choose the same location twice

	errlocs[errloc] = 1;

	enc.data[errloc] ^= errval;		// cause the error
      }

      // decode the errored block
      derrors = rs.decode (out, enc);

      if (errors <= NROOTS/2) {
	// We should have handled all these errors and corrected them.
	if (derrors != errors){
	  fprintf (stderr, "  decoder says %d errors, true number is %d\n", derrors, errors);
	  decoder_errors++;
	}

	if (in != out){
	  fprintf (stderr, "  uncorrected errors!\n");
	  decoder_errors++;
	}
      } else {
	// We have been given more errors than we could cope with.  Make
	// sure that we detect these errors.  Complain if we get incorrect
	// block but don't say it's incorrect.
	bool differs = (in != out);

	if (differs && (derrors < 0)) {
	  // Reported uncorrectable error accurately
	} else if (differs) {
	  fprintf (stderr,
		   "  decoder found %d of %d errors, but incorrect block\n",
		   derrors, errors);
	} else {
	  fprintf (stderr, "  decoder corrected %d of %d errors unexpectedly\n",
		   derrors, errors);
	}
      }
    }
  }

  BOOST_REQUIRE(decoder_errors == 0);
}
