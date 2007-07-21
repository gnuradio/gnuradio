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

#include <iostream>
#include <stdio.h>

using std::cerr;

/*
 * Trellis-encode a whole pile of 12 data segments for ATSC.
 * This also includes scrambling the data among twelve Trellis encoders.
 *
 * Input is twelve 207-byte blocks of raw data (Reed-Solomon output that's
 * been scrambled up by interleaving with other blocks).
 *
 * Output is 12 x 208 x 4 bytes, each byte containing a 3-bit symbol.
 * The first 4 bytes are the segment sync symbol.
 *
    Got the first version of Trellis encoder coded.  Compiles, but I
    didn't realize that each data segment contains an odd number of BITS!
    The second data segment in a field starts by pulling bits out of the
    middles of the bytes it's encoding.  You actually have to read all the
    entries in that table on page 59 AND 60 to get it.

    There's a 4-segment asymmetric pattern of bit accesses.
    There's a 3-segment asymmetric pattern of muxing encoders.

    The result is there's a 12-segment pattern that repeats throughout
    the encoding of a field.  So this routine now encodes 12 segments at once.

    This encoding system was either designed by a complete idiot or by
    a complete genius.  It's highly complex when it could have been very
    simple.  Now the question is whether
    this incredible complexity buys us anything subtle and important.
 */

#define SEGMENT_SIZE	207
#define	INPUT_SIZE	(SEGMENT_SIZE * 12)
#define	DIBITS_PER_BYTE	4
#define	EXTRAS		(4 * 12)	/* FIXME, sync symbols and such */
#define	SYMBOLS_OUT	((INPUT_SIZE * DIBITS_PER_BYTE) + EXTRAS)
#define SEGOF(x)	( (x) / ((SEGMENT_SIZE+1) * DIBITS_PER_BYTE))
#define SYMOF(x)	(((x) % ((SEGMENT_SIZE+1) * DIBITS_PER_BYTE))-4)
#define	ENCODERS	12
#define	ENCODER_SEG_BUMP	4


/* Shift counts to bit numbers (high order, low order); 9x entries unused */
static const int bit1[8] = {1, 99, 3, 98, 5, 97, 7, 96};
static const int bit2[8] = {0, 99, 2, 98, 4, 97, 6, 96};

/* Detailed Debugging */
int debug_dec = 0;

/* 
 * Build indirect data structures to say which symbols go into which
 * encoder, and then where the resulting dibits from the encoders go.
 */
int
build_decode_structures (char *fileout)
{
  int retval = 0;
  int i;
  int encoder;
  int trellis_wheredata[ENCODERS];
  unsigned char *symp, *next_sym_seg;
  unsigned char symbols[SYMBOLS_OUT];
  int chunk;
  int shift;
  int skip_encoder_bump;
  int *enco_syms[ENCODERS];
  int *enco_dibits[ENCODERS];
  int j;
  /* The data structures we'll build and then spit out... */
  int sync_symbol_indices[1000];
  int sync_symbol_indices_max;
  int enco_which_syms[ENCODERS][INPUT_SIZE];
  int enco_which_dibits[ENCODERS][INPUT_SIZE];
  int enco_which_max;
  #define	BIT_PTR(int,shif) (((int) << 3) | ((shif) & 0x7))
  /* Running indices into them as we build 'em... */
  int *syncsyms = sync_symbol_indices;

  /* Start our running pointers at the start of our per-encoder subarrays */
  for (i = 0; i < ENCODERS; i++) {
    enco_dibits[i] = enco_which_dibits[i];
    enco_syms[i] = enco_which_syms[i];
  }

  encoder = ENCODERS - ENCODER_SEG_BUMP;
  skip_encoder_bump = 0;
  symp = symbols;
  next_sym_seg = symp;

  for (chunk = 0;
       chunk < INPUT_SIZE;
       chunk += ENCODERS) {
    /* Associate data bytes with the Trellis encoders.
       They get loaded or stored in an order that depends on where we are in the
       segment sync progress (sigh). 
       GRR!  When the chunk reload happens at the same time as the
       segment boundary, we should bump the encoder NOW for the reload,
       rather than LATER during the bitshift transition!!! */
    if (symp >= next_sym_seg) {
      encoder = (encoder + ENCODER_SEG_BUMP) % ENCODERS;
      skip_encoder_bump = 1;
    }
      
    /* Remember where the data bytes are going to go, once we've
       accumulated them from the 12 interleaved decoders */
    for (i = 0; i < ENCODERS; i++) {
      trellis_wheredata[encoder] = chunk+i;
      encoder++;
      if (encoder >= ENCODERS) encoder = 0;
    }

    for (shift = 6; shift >= 0; shift -= 2) {

      /* Segment boundaries happen to occur on some bitshift transitions. */
      if (symp >= next_sym_seg) {
	/* Segment transition.  Output a data segment sync symbol, and
	   mess with the trellis encoder mux.  */
	*syncsyms++ = symp - symbols;
	symp += 4;
        next_sym_seg = symp + (SEGMENT_SIZE * DIBITS_PER_BYTE);

	if (!skip_encoder_bump)
	  encoder = (encoder + ENCODER_SEG_BUMP) % ENCODERS;
	skip_encoder_bump = 0;
      }

      /* Now run each of the 12 Trellis encoders to spit out 12 symbols.
         Each encoder takes input from the same byte of the chunk, but the
         outputs of the encoders come out in various orders.
         NOPE -- this is false.  The encoders take input from various
         bytes of the chunk (which changes at segment sync time), AND
         they also come out in various orders.  You really do have to
         keep separate track of:  the datasegs bytes, the encoders, and
         the symbol bytes -- because they're all moving with respect to
         each other!!!  */
      for (i = 0; i < ENCODERS; i++) {
        if (debug_dec)
	  printf ("Seg %ld Symb %3ld Trell %2d Byte %6d Bits %d-%d = ",
		  (long) SEGOF(symp-symbols), (long) SYMOF(symp-symbols),
		  encoder, trellis_wheredata[encoder],
		  bit1[shift], bit2[shift]);

	/* Decoding:  Grab symbol, run through decoder, slice dibit into
	   buffer.  */
	/* This symbol goes into this encoder next */
	*(enco_syms[encoder]++) = symp - symbols;
        symp++;
	/* The next output from this encoder goes into these dibits */
	*(enco_dibits[encoder]++) = BIT_PTR(trellis_wheredata[encoder], shift);

        encoder++; if (encoder >= ENCODERS) encoder = 0;
      } /* Encoders */
    } /* Bit shifts */
      
#if 0
    /* Now dump out the chunk of 12 data bytes that the twelve decoders have
       accumulated. */
  unsigned char trellis_buffer[ENCODERS];
  unsigned char dibit;
  unsigned char symbol;
  int save_state;
    for (i = 0; i < ENCODERS; i++) {
      datasegs [trellis_wheredata[encoder]] = trellis_buffer[encoder];
      encoder++;
      if (encoder >= ENCODERS) encoder = 0;
    } /* Dumping encoder bytes */
#endif
  } /* Chunks */
  
  /* Now print the resulting data structures in C++ */

  if (!freopen(fileout, "w", stdout))
    return 2;

  printf ("/*\n\
 * atsc_viterbi_mux.cc\n\
 *\n\
 * Data structures for knowing which symbols are fed to which\n\
 * Viterbi decoders, and then where to put the resulting output dibits.\n\
 *\n\
 * Generated by 'atsc_viterbi_gen.cc'.\n\
 */\n\n");
  sync_symbol_indices_max = syncsyms - sync_symbol_indices;
  printf ("const unsigned int sync_symbol_indices_max = %d;\n",
	  sync_symbol_indices_max);
  printf ("const unsigned int sync_symbol_indices[%d] = {\n  ",
	  sync_symbol_indices_max);
  for (i = 0; i < sync_symbol_indices_max; i++) {
    printf ("%d,%s", sync_symbol_indices[i], (7 == i%8)? "\n  ": " ");
  }
  printf ("};\n\n");

  enco_which_max = enco_dibits[0] - enco_which_dibits[0];
  for (i = 0; i < ENCODERS; i++) 
    if (enco_which_max != enco_dibits[i] - enco_which_dibits[i]) {
      cerr << "Encoder " << i << " has different max_dibits " <<
	enco_dibits[i] - enco_which_dibits[i] << " than " << enco_which_max;
      retval = 3;
    }

  printf ("const unsigned int enco_which_max = %d;\n" , enco_which_max);

  printf ("const unsigned int enco_which_syms[%d][%d] = {\n",
	  ENCODERS, enco_which_max);
  for (i = 0; i < ENCODERS; i++) {
    printf ("  /* %d */\n  {", i);
    for (j = 0; j < enco_which_max; j++)
      printf ("%d,%s", enco_which_syms[i][j], (7 == j%8)? "\n   ": " ");
    printf ("},\n");
  }
  printf ("};\n\n");

  printf ("const unsigned int enco_which_dibits[%d][%d] = {\n",
	  ENCODERS, enco_which_max);
  for (i = 0; i < ENCODERS; i++) {
    printf ("  /* %d */\n  {", i);
    for (j = 0; j < enco_which_max; j++)
      printf ("%d,%s", enco_which_dibits[i][j], (7 == j%8)? "\n   ": " ");
    printf ("},\n");
  }
  printf ("};\n\n");
  return retval;
}

int
usage()
{ 
  cerr << "atsc_viterbi_gen: Usage:\n";
  cerr << "  ./atsc_viterbi_gen -o atsc_viterbi_mux.cc\n";
  cerr << "That's all, folks!\n";
  return 1;
}


int
main(int argc, char **argv) 
{
  if (argc != 3) return usage();
  if (argv[1][0] != '-'
   || argv[1][1] != 'o'
   || argv[1][2] != 0   ) return usage();
  return build_decode_structures(argv[2]);
}
