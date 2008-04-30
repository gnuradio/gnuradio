/* -*- c++ -*- */
/*
 * Copyright 2002,2006 Free Software Foundation, Inc.
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

#include <atsci_trellis_encoder.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static const int DIBITS_PER_BYTE = 4;

#define SEGOF(x)	( (x) / ((SEGMENT_SIZE+1) * DIBITS_PER_BYTE))
#define SYMOF(x)	(((x) % ((SEGMENT_SIZE+1) * DIBITS_PER_BYTE))-4)

/* How many separate Trellis encoders / Viterbi decoders run in parallel */
static const int	NCODERS = 12;

#define	ENCODER_SEG_BUMP	4

/* A Segment sync symbol is an 8VSB +5,-5,-5,+5 sequence that occurs at
   the start of each 207-byte segment (including field sync segments).  */
#define	DSEG_SYNC_SYM1	0x06	/* +5 */
#define	DSEG_SYNC_SYM2	0x01	/* -5 */
#define	DSEG_SYNC_SYM3	0x01	/* -5 */
#define	DSEG_SYNC_SYM4	0x06	/* +5 */


/* Shift counts to bit numbers (high order, low order); 9x entries unused */
static const int bit1[8] = {1, 99, 3, 98, 5, 97, 7, 96};
static const int bit2[8] = {0, 99, 2, 98, 4, 97, 6, 96};


atsci_trellis_encoder::atsci_trellis_encoder ()
{
  debug = false;
  reset ();
}

atsci_trellis_encoder::~atsci_trellis_encoder ()
{
}

void
atsci_trellis_encoder::reset ()
{
  for (int i = 0; i < NCODERS; i++)
    enc[i].reset ();
}

void 
atsci_trellis_encoder::encode (atsc_data_segment out[NCODERS],
			      const atsc_mpeg_packet_rs_encoded in[NCODERS])
{
  unsigned char out_copy[OUTPUT_SIZE];
  unsigned char in_copy[INPUT_SIZE];

  assert (sizeof (in_copy) == sizeof (in[0].data) * NCODERS);
  assert (sizeof (out_copy) == sizeof (out[0].data) * NCODERS);

  // copy input into continguous temporary buffer
  for (int i = 0; i < NCODERS; i++){
    assert (in[i].pli.regular_seg_p ());
    plinfo::sanity_check (in[i].pli);
    memcpy (&in_copy[i * INPUT_SIZE/NCODERS],
	    &in[i].data[0],
	    ATSC_MPEG_RS_ENCODED_LENGTH * sizeof (in_copy[0]));
  }

  memset (out_copy, 0, sizeof (out_copy));	// FIXME, sanity check
  
  // do the deed...
  encode_helper (out_copy, in_copy);

  // copy output from contiguous temp buffer into final output
  for (int i = 0; i < NCODERS; i++){
    memcpy (&out[i].data[0],
	    &out_copy[i * OUTPUT_SIZE/NCODERS],
	    ATSC_DATA_SEGMENT_LENGTH * sizeof (out_copy[0]));

    // copy pipeline info
    out[i].pli = in[i].pli;

    plinfo::sanity_check (out[i].pli);
    assert (out[i].pli.regular_seg_p ());
  }
}

/*
 * This code expects contiguous arrrays.  Use it as is, it computes
 * the correct answer.  Maybe someday, when we've run out of better
 * things to do, rework to avoid the copying in encode.
 */
void 
atsci_trellis_encoder::encode_helper (unsigned char output[OUTPUT_SIZE],
				     const unsigned char input[INPUT_SIZE])
{
  int i;
  int encoder;
  unsigned char trellis_buffer[NCODERS];
  int trellis_wherefrom[NCODERS];
  unsigned char *out, *next_out_seg;
  int chunk;
  int shift;
  unsigned char dibit;
  unsigned char symbol;
  int skip_encoder_bump;

  /* FIXME, we may want special processing here 
     for a flag byte to keep track of which part of the field we're in? */

  encoder = NCODERS - ENCODER_SEG_BUMP;
  skip_encoder_bump = 0;
  out = output;
  next_out_seg = out;

  for (chunk = 0;
       chunk < INPUT_SIZE;
       chunk += NCODERS) {
    /* Load a new chunk of bytes into the Trellis encoder buffers.
       They get loaded in an order that depends on where we are in the
       segment sync progress (sigh). 
       GRR!  When the chunk reload happens at the same time as the
       segment boundary, we should bump the encoder NOW for the reload,
       rather than LATER during the bitshift transition!!! */
    if (out >= next_out_seg) {
      encoder = (encoder + ENCODER_SEG_BUMP) % NCODERS;
      skip_encoder_bump = 1;
    }
      
    for (i = 0; i < NCODERS; i++) {
      /* for debug */ trellis_wherefrom[encoder] = chunk+i;
      trellis_buffer[encoder] =             input [chunk+i];
      encoder++;
      if (encoder >= NCODERS) encoder = 0;
    }

    for (shift = 6; shift >= 0; shift -= 2) {

      /* Segment boundaries happen to occur on some bitshift transitions. */
      if (out >= next_out_seg) {
	/* Segment transition.  Output a data segment sync symbol, and
	   mess with the trellis encoder mux.  */
	*out++ = DSEG_SYNC_SYM1;
	*out++ = DSEG_SYNC_SYM2;
	*out++ = DSEG_SYNC_SYM3;
	*out++ = DSEG_SYNC_SYM4;
	if (debug) printf ("SYNC SYNC SYNC SYNC\n");
        next_out_seg = out + (SEGMENT_SIZE * DIBITS_PER_BYTE);

	if (!skip_encoder_bump)
	  encoder = (encoder + ENCODER_SEG_BUMP) % NCODERS;
	skip_encoder_bump = 0;
      }

      /* Now run each of the 12 Trellis encoders to spit out 12 symbols.
         Each encoder takes input from the same byte of the chunk, but the
         outputs of the encoders come out in various orders.
         NOPE -- this is false.  The encoders take input from various
         bytes of the chunk (which changes at segment sync time), AND
         they also come out in various orders.  You really do have to
         keep separate track of:  the input bytes, the encoders, and
         the output bytes -- because they're all moving with respect to
         each other!!!  */
      for (i = 0; i < NCODERS; i++) {
	dibit = 0x03 & (trellis_buffer[encoder] >> shift);
        if (debug)
	  printf ("Seg %ld Symb %3ld Trell %2d Byte %6d Bits %d-%d = dibit %d ",
		  (long) SEGOF(out-output), (long) SYMOF(out-output),
		  encoder, trellis_wherefrom[encoder],
		  bit1[shift], bit2[shift], dibit);
	symbol = enc[encoder].encode (dibit);
	*out++ = symbol;
        encoder++; if (encoder >= NCODERS) encoder = 0;
	if (debug) printf ("sym %d\n", symbol);
      } /* Encoders */
    } /* Bit shifts */
  } /* Chunks */
  
  /* Check up on ourselves */
#if 0
  assertIntsEqual (0, (INPUT_SIZE * DIBITS_PER_BYTE) % NCODERS, "not %");
  assertIntsEqual (OUTPUT_SIZE, out - output, "outptr");
  assertIntsEqual (NCODERS - ENCODER_SEG_BUMP, encoder, "mux sync");
#else
  assert (0 ==  (INPUT_SIZE * DIBITS_PER_BYTE) % NCODERS);
  assert (OUTPUT_SIZE == out - output);
  assert (NCODERS - ENCODER_SEG_BUMP == encoder);
#endif
}

