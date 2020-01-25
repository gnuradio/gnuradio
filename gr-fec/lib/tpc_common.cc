/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/fec/tpc_common.h>

namespace gr {
namespace fec {

int tpc_common::parity_counter(int symbol, int length)
{
    int counter;
    int temp_parity = 0;

    for (counter = 0; counter < length; counter++) {
        temp_parity = temp_parity ^ (symbol & 1);
        symbol = symbol >> 1;
    }

    return (temp_parity);
}

void tpc_common::rsc_enc_bit(int input,
                             int state_in,
                             std::vector<int> g,
                             int KK,
                             int nn,
                             std::vector<std::vector<int>>& outputVec,
                             std::vector<std::vector<int>>& nextStateVec)
{
    int state, i, out, a_k;

    // systematic output
    out = input;

    // determine feedback bit
    a_k = input ^ tpc_common::parity_counter(g[0] & state_in, KK);

    // create a word made up of state and feedback bit
    state = (a_k << (KK - 1)) ^ state_in;

    // AND the word with the generators
    for (i = 1; i < nn; i++) {
        // update output symbol
        out = (out << 1) + tpc_common::parity_counter(state & g[i], KK);
    }

    outputVec[input][state_in] = out;
    nextStateVec[input][state_in] = (state >> 1);
}

void tpc_common::precomputeStateTransitionMatrix_RSCPoly(
    int numStates,
    std::vector<int> g,
    int KK,
    int nn,
    std::vector<std::vector<int>>& output,
    std::vector<std::vector<int>>& nextStates)
{

    for (int input = 0; input < 2; input++) {
        for (int state = 0; state < numStates; state++) {
            tpc_common::rsc_enc_bit(input, state, g, KK, nn, output, nextStates);
        }
    }
}

void tpc_common::rsc_tail(std::vector<int>& tail_p,
                          std::vector<int> g,
                          int max_states,
                          int mm)
{
    // Determine the tail for each state
    for (int state = 0; state < max_states; state++) {
        // determine feedback word
        tail_p[state] = tpc_common::parity_counter(g[0] & state, mm);
    }
    return;
}

void tpc_common::itob(std::vector<int>& binVec, int symbol, int length)
{
    /// Go through each bit in the vector
    for (int counter = 0; counter < length; counter++) {
        binVec[length - counter - 1] = (symbol & 1);
        symbol = symbol >> 1;
    }
}

} // namespace fec
} // namespace gr
