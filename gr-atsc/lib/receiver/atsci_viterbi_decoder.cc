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

#include <gnuradio/atsc/viterbi_decoder.h>
#include <assert.h>
#include <stdio.h>
#include <cmath>
#include "atsci_viterbi_mux.cc"
#include <string.h>


/* How many separate Trellis encoders / Viterbi decoders run in parallel */
static const int	NCODERS = 12;

static const float	DSEG_SYNC_SYM1 =  5;
static const float	DSEG_SYNC_SYM2 = -5;
static const float	DSEG_SYNC_SYM3 = -5;
static const float	DSEG_SYNC_SYM4 =  5;

atsci_viterbi_decoder::atsci_viterbi_decoder ()
{
  debug = true;

  /*
   * These fifo's handle the alignment problem caused by the
   * inherent decoding delay of the individual viterbi decoders.
   * The net result is that this entire block has a pipeline latency
   * of 12 complete segments.
   *
   * If anybody cares, it is possible to do it with less delay, but
   * this approach is at least somewhat understandable...
   */

  // the -4 is for the 4 sync symbols
  int	fifo_size = ATSC_DATA_SEGMENT_LENGTH - 4 - viterbi[0].delay ();
  for (int i = 0; i < NCODERS; i++)
    fifo[i] = new fifo_t(fifo_size);

  reset ();
}

atsci_viterbi_decoder::~atsci_viterbi_decoder ()
{
  for (int i = 0; i < NCODERS; i++)
    delete fifo[i];
}

void
atsci_viterbi_decoder::reset ()
{
  for (int i = 0; i < NCODERS; i++){
    viterbi[i].reset ();
    fifo[i]->reset ();
  }
}


void
atsci_viterbi_decoder::decode (atsc_mpeg_packet_rs_encoded out[NCODERS],
			      const atsc_soft_data_segment in[NCODERS])
{
  unsigned char out_copy[OUTPUT_SIZE];
  float 	in_copy[INPUT_SIZE];

  // copy input into continguous temporary buffer
  for (int i = 0; i < NCODERS; i++){
    assert (in[i].pli.regular_seg_p ());
    memcpy (&in_copy[i * INPUT_SIZE/NCODERS],
	    &in[i].data[0],
	    ATSC_DATA_SEGMENT_LENGTH * sizeof (in_copy[0]));
  }

  memset (out_copy, 0, sizeof (out_copy));	// sanity check

  // do the deed...
  decode_helper (out_copy, in_copy);

  // copy output from contiguous temp buffer into final output
  for (int i = 0; i < NCODERS; i++){
    memcpy (&out[i].data[0],
	    &out_copy[i * OUTPUT_SIZE/NCODERS],
	    ATSC_MPEG_RS_ENCODED_LENGTH * sizeof (out_copy[0]));


    // adjust pipeline info to reflect 12 segment delay
    plinfo::delay (out[i].pli, in[i].pli, NCODERS);
  }
}

void
atsci_viterbi_decoder::decode_helper (unsigned char out[OUTPUT_SIZE],
				     const float symbols_in[INPUT_SIZE])
{
  int encoder;
  unsigned int i;
  int dbi;
  int dbwhere;
  int dbindex;
  int shift;
  unsigned char dibit;
  float symbol;

  /* Memset is not necessary if it's all working... */
  memset (out, 0, OUTPUT_SIZE);

#define VERBOSE 0

#if 1
  /* Check for sync symbols in the incoming data stream */
  /* For now, all we do is complain to stdout.  FIXME, pass it back to
     caller as an error/quality signal.  */
  for (i = 0; i < sync_symbol_indices_max; i++) {
    int j = sync_symbol_indices[i];
    if (fabsf (symbols_in[j] - DSEG_SYNC_SYM1) > 1.0)
      if (VERBOSE) fprintf (stderr, "** Missing dataseg sync 1 at %d, expect %g, got %g.\n",
			    j, DSEG_SYNC_SYM1, symbols_in[j]);
    j++;
    if (fabsf (symbols_in[j] - DSEG_SYNC_SYM2) > 1.0)
      if (VERBOSE) fprintf (stderr, "** Missing dataseg sync 2 at %d, expect %g, got %g.\n",
			    j, DSEG_SYNC_SYM2, symbols_in[j]);
    j++;
    if (fabsf (symbols_in[j] - DSEG_SYNC_SYM3) > 1.0)
      if (VERBOSE) fprintf (stderr, "** Missing dataseg sync 3 at %d, expect %g, got %g.\n",
			    j, DSEG_SYNC_SYM3, symbols_in[j]);
    j++;
    if (fabsf (symbols_in[j] - DSEG_SYNC_SYM4) > 1.0)
      if (VERBOSE) fprintf (stderr, "** Missing dataseg sync 4 at %d, expect %g, got %g.\n",
			    j, DSEG_SYNC_SYM4, symbols_in[j]);
  }
#endif
#undef VERBOSE

  // printf ("@@@ DIBITS @@@\n");

  /* Now run each of the 12 Trellis encoders over their subset of
     the input symbols */
  for (encoder = 0; encoder < NCODERS; encoder++) {
    dbi = 0;			/* Reinitialize dibit index for new encoder */
    fifo_t	*dibit_fifo = fifo[encoder];

    /* Feed all the incoming symbols into one encoder;
       pump them into the relevant dibits. */
    for (i = 0; i < enco_which_max; i++) {
      symbol = symbols_in[enco_which_syms[encoder][i]];
      dibit = dibit_fifo->stuff (viterbi[encoder].decode (symbol));
      // printf ("%d\n", dibit);
      /* Store the dibit into the output data segment */
      dbwhere = enco_which_dibits[encoder][dbi++];
      dbindex = dbwhere >> 3;
      shift = dbwhere & 0x7;
      out[dbindex] =
	(out[dbindex] & ~(0x03 << shift)) | (dibit << shift);
    } /* Symbols fed into one encoder */
  } /* Encoders */

  // fflush (stdout);
}
