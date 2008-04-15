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
 * This is a minimal example demonstrating how to call the ECC encoder
 * in continuous streaming mode.  It accepts data on stdin and writes to 
 * stdout.
 * 
 * FIXME: This does not flush the final bits out of the encoder.
 *
 */

extern "C" {
#include "viterbi.h"
}

#include <cstdio>

#define MAXCHUNKSIZE 4096
#define MAXENCSIZE MAXCHUNKSIZE*16

int main()
{
  unsigned char encoder_state = 0;
  unsigned char data[MAXCHUNKSIZE];
  unsigned char syms[MAXENCSIZE];
  
  while (!feof(stdin)) {
    unsigned int n = fread(data, 1, MAXCHUNKSIZE, stdin);
    encoder_state = encode(syms, data, n, encoder_state);
    fwrite(syms, 1, n*16, stdout);
  }

  return 0;
}
