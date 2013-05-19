/* -*- c++ -*- */
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

/*
 * This is a minimal example demonstrating how to call the Viterbi decoder
 * in continuous streaming mode.  It accepts data on stdin and writes to
 * stdout.
 *
 */

extern "C" {
#include <gnuradio/fec/viterbi.h>
}

#include <cstdio>
#include <cmath>

#define MAXCHUNKSIZE 4096
#define MAXENCSIZE MAXCHUNKSIZE*16

int main()
{
  unsigned char data[MAXCHUNKSIZE];
  signed char syms[MAXENCSIZE];
  int count = 0;

  // Initialize metric table
  int mettab[2][256];
  int amp = 100;
  float RATE=0.5;
  float ebn0 = 12.0;
  float esn0 = RATE*pow(10.0, ebn0/10);
  gen_met(mettab, amp, esn0, 0.0, 4);

  // Initialize decoder state
  struct viterbi_state state0[64];
  struct viterbi_state state1[64];
  unsigned char viterbi_in[16];
  viterbi_chunks_init(state0);

  while (!feof(stdin)) {
    unsigned int n = fread(syms, 1, MAXENCSIZE, stdin);
    unsigned char *out = data;

    for (unsigned int i = 0; i < n; i++) {

      // FIXME: This implements hard decoding by slicing the input stream
      unsigned char sym = syms[i] > 0 ? -amp : amp;

      // Write the symbol to the decoder input
      viterbi_in[count % 4] = sym;

      // Every four symbols, perform the butterfly2 operation
      if ((count % 4) == 3) {
        viterbi_butterfly2(viterbi_in, mettab, state0, state1);

	// Every sixteen symbols, perform the readback operation
        if ((count > 64) && (count % 16) == 11) {
          viterbi_get_output(state0, out);
	  fwrite(out++, 1, 1, stdout);
	}
      }

      count++;
    }
  }

  return 0;
}
