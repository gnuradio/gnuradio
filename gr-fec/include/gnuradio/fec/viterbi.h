/*
 * Copyright 2008 Free Software Foundation, Inc.
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

/* The path memory for each state is 32 bits. This is slightly shorter
 * than we'd like for K=7, especially since we chain back every 8 bits.
 * But it fits so nicely into a 32-bit machine word...
 */

#include <gnuradio/fec/api.h>

struct viterbi_state {
  unsigned long path;	/* Decoded path to this state */
  long metric;		/* Cumulative metric to this state */
};

FEC_API
int gen_met(int mettab[2][256],	/* Metric table */
	    int amp,		/* Signal amplitude */
	    double esn0,	/* Es/N0 ratio in dB */
	    double bias, 	/* Metric bias */
	    int scale);		/* Scale factor */

FEC_API unsigned char
encode(unsigned char *symbols, unsigned char *data,
       unsigned int nbytes,unsigned char encstate);

FEC_API void
viterbi_chunks_init(struct viterbi_state* state);

 FEC_API void
viterbi_butterfly2(unsigned char *symbols, int mettab[2][256],
		   struct viterbi_state *state0, struct viterbi_state *state1);

FEC_API unsigned char
viterbi_get_output(struct viterbi_state *state, unsigned char *outbuf);
