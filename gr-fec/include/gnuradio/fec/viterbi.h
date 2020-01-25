/*
 * Copyright 2008,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* The path memory for each state is 32 bits. This is slightly shorter
 * than we'd like for K=7, especially since we chain back every 8 bits.
 * But it fits so nicely into a 32-bit machine word...
 */

#ifndef INCLUDED_VITERBI_H
#define INCLUDED_VITERBI_H

#include <gnuradio/fec/api.h>

namespace gr {
namespace fec {

struct FEC_API viterbi_state {
    unsigned long path; /* Decoded path to this state */
    long metric;        /* Cumulative metric to this state */
};

FEC_API
void gen_met(int mettab[2][256], /* Metric table */
             int amp,            /* Signal amplitude */
             double esn0,        /* Es/N0 ratio in dB */
             double bias,        /* Metric bias */
             int scale);         /* Scale factor */

FEC_API unsigned char encode(unsigned char* symbols,
                             unsigned char* data,
                             unsigned int nbytes,
                             unsigned char encstate);

FEC_API void viterbi_chunks_init(struct viterbi_state* state);

FEC_API void viterbi_butterfly2(unsigned char* symbols,
                                int mettab[2][256],
                                struct viterbi_state* state0,
                                struct viterbi_state* state1);

FEC_API unsigned char viterbi_get_output(struct viterbi_state* state,
                                         unsigned char* outbuf);
} // namespace fec
} // namespace gr

#endif /* INCLUDED_VITERBI_H */
