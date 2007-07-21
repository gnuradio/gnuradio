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
#include <qa_atsci_viterbi_decoder.h>
#include <qa_atsci_trellis_encoder.h>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define	NELEM(x) (sizeof (x) / sizeof (x[0]))


static const int NCODERS = atsci_viterbi_decoder::NCODERS;

#if 0
static void
map_to_soft_symbols (atsc_soft_data_segment &out,
		     const atsc_data_segment &in)
{
  for (unsigned int i = 0; i < NELEM (in.data); i++){
    out.data[i] = in.data[i] * 2 - 7;
  }
}
#endif

static void
pad_decoder_input (atsc_soft_data_segment out[NCODERS])
{
  memset (out,  0, sizeof (out));

  // add data segment sync
  for (int i = 0; i < NCODERS; i++){
    out[i].data[0] =  5;
    out[i].data[1] = -5;
    out[i].data[2] = -5;
    out[i].data[3] =  5;
    out[i].pli.set_regular_seg (false, i);
  }
}

void
qa_atsci_viterbi_decoder::t0 ()
{
#if 0
  atsci_trellis_encoder		enc;
  atsc_mpeg_packet_rs_encoded	encoder_in[NCODERS];
  atsc_data_segment		encoder_out[NCODERS];
  atsc_soft_data_segment	decoder_in[NCODERS];
  atsc_soft_data_segment	decoder_in_pad[NCODERS];
  atsc_mpeg_packet_rs_encoded	decoder_out[NCODERS];
  atsc_mpeg_packet_rs_encoded	decoder_out_pad[NCODERS];

  
  memset (encoder_in,      0, sizeof (encoder_in));
  memset (encoder_out,     0, sizeof (encoder_out));
  memset (decoder_out_pad, 0, sizeof (decoder_out_pad));

  srandom (1);

  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (encoder_in[i].data); j++){
      int t = (random () >> 8) & 0xff;	// 8 random bits
      encoder_in[i].data[j] = t;
    }
  }
  
  fflush (stdout);
  printf ("@@@ ENCODER INPUT @@@\n");
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (encoder_in[i].data); j++){
       printf ("%d\n", encoder_in[i].data[j]);
    }
  }

  enc.reset ();
  enc.encode (encoder_out, encoder_in);

  printf ("@@@ ENCODER OUTPUT @@@\n");
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (encoder_out[i].data); j++){
       printf ("%d\n", encoder_out[i].data[j]);
    }
  }

  for (int i = 0; i < NCODERS; i++)
    map_to_soft_symbols (decoder_in[i], encoder_out[i]);

  viterbi.reset ();

  // this has only the previous (non-existant) output
  viterbi.decode (decoder_out_pad, decoder_in);	

  // now we'll see the real output
  pad_decoder_input (decoder_in_pad);
  viterbi.decode (decoder_out, decoder_in_pad);

  printf ("@@@ DECODER OUTPUT @@@\n");
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (decoder_out[i].data); j++){
       printf ("%d\n", decoder_out[i].data[j]);
    }
  }
  fflush (stdout);
#endif
}

void
qa_atsci_viterbi_decoder::t1 ()
{
  atsc_soft_data_segment	decoder_in[NCODERS];
  atsc_soft_data_segment	decoder_in_pad[NCODERS];
  atsc_mpeg_packet_rs_encoded	decoder_out[NCODERS];
  atsc_mpeg_packet_rs_encoded	decoder_out_pad[NCODERS];
  atsc_mpeg_packet_rs_encoded	expected_out[NCODERS];
  static const float 		raw_input[NCODERS * NELEM (decoder_in[0].data)] = {
#include "qa_atsci_viterbi_decoder_t1_input.dat"
  };
  static const unsigned char	raw_output[NCODERS * NELEM (expected_out[0].data)] = {
#include "qa_atsci_viterbi_decoder_t1_output.dat"
  };


  // load up input
  const float *ri = &raw_input[0];
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (decoder_in[i].data); j++){
      decoder_in[i].data[j] = *ri++;
    }
    decoder_in[i].pli.set_regular_seg (false, i);
  }

  // load up expected output
  const unsigned char *ro = &raw_output[0];
  for (int i = 0; i < NCODERS; i++){
    for (unsigned int j = 0; j < NELEM (expected_out[i].data); j++){
      expected_out[i].data[j] = *ro++;
    }
    expected_out[i].pli.set_regular_seg (false, i);
  }

  viterbi.reset ();

  // this has only the previous (non-existant) output
  viterbi.decode (decoder_out_pad, decoder_in);	

  // now we'll see the real output
  pad_decoder_input (decoder_in_pad);
  viterbi.decode (decoder_out, decoder_in_pad);

  for (int i = 0; i < NCODERS; i++){			// check the result
    CPPUNIT_ASSERT (expected_out[i] == decoder_out[i]);
  }
}
