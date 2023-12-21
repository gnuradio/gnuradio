/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * This is a minimal example demonstrating how to call the ECC encoder
 * in continuous streaming mode.  It accepts data on stdin and writes to
 * stdout.
 *
 * FIXME: This does not flush the final bits out of the encoder.
 *
 */

#include <gnuradio/fec/viterbi.h>

#include <cstdio>

#define MAXCHUNKSIZE 4096
#define MAXENCSIZE MAXCHUNKSIZE * 16

int main()
{
    unsigned char encoder_state = 0;
    unsigned char data[MAXCHUNKSIZE];
    unsigned char syms[MAXENCSIZE];

    while (!feof(stdin)) {
        unsigned int n = fread(data, 1, MAXCHUNKSIZE, stdin);
        encoder_state = gr::fec::encode(syms, data, n, encoder_state);
        fwrite(syms, 1, n * 16, stdout);
    }

    return 0;
}
