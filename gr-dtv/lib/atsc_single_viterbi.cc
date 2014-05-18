/* -*- c++ -*- */
/*
 * Copyright 2002, 2014 Free Software Foundation, Inc.
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

#include "atsc_single_viterbi.h"
#include <math.h>

namespace gr {
  namespace dtv {

    /* was_sent is a table of what symbol we get given what bit pair
       was sent and what state we where in [state][pair] */
    const int atsc_single_viterbi::was_sent[8][4] = {
      {0,2,0,2},
      {0,2,0,2},
      {1,3,1,3},
      {1,3,1,3},
      {4,6,4,6},
      {4,6,4,6},
      {5,7,5,7},
      {5,7,5,7}
    };

    /* transition_table is a table of what state we were in
       given current state and bit pair sent [state][pair] */
    const int atsc_single_viterbi::transition_table[8][4] = {
      {0,2,4,6},
      {2,0,6,4},
      {1,3,5,7},
      {3,1,7,5},
      {4,6,0,2},
      {6,4,2,0},
      {5,7,1,3},
      {7,5,3,1}
    };

    void
    atsc_single_viterbi::reset()
    {
      for (unsigned int i = 0; i<2; i++)
        for (unsigned int j = 0; j<8; j++) {
          path_metrics[i][j] = 0;
          traceback[i][j] = 0;
        }

      phase = 0;
    }

    atsc_single_viterbi::atsc_single_viterbi()
    {
      reset();
    }

    char
    atsc_single_viterbi::decode(float input)
    {
      unsigned int best_state = 0;
      float best_state_metric = 100000;

      /* Precompute distances from input to each possible symbol */
      float distances[8] = { (float)fabs( input + 7 ), (float)fabs( input + 5 ),
                             (float)fabs( input + 3 ), (float)fabs( input + 1 ),
                             (float)fabs( input - 1 ), (float)fabs( input - 3 ),
                             (float)fabs( input - 5 ), (float)fabs( input - 7 ) };

     /* We start by iterating over all possible states */
      for (unsigned int state = 0; state < 8; state++) {
        /* Next we find the most probable path from the previous
           states to the state we are testing, we only need to look at
           the 4 paths that can be taken given the 2-bit input */
        int min_metric_symb = 0;
        float min_metric = distances[was_sent[state][0]] + path_metrics[phase][transition_table[state][0]];
        for (unsigned int symbol_sent = 1; symbol_sent < 4; symbol_sent++)
          if( (distances[was_sent[state][symbol_sent]] + path_metrics[phase][transition_table[state][symbol_sent]]) < min_metric) {
            min_metric = distances[was_sent[state][symbol_sent]] + path_metrics[phase][transition_table[state][symbol_sent]];
            min_metric_symb = symbol_sent;
          }

        path_metrics[phase^1][state] = min_metric;
        traceback[phase^1][state] = (((unsigned long long)min_metric_symb) << 62) | (traceback[phase][transition_table[state][min_metric_symb]] >> 2);

        /* If this is the most probable state so far remember it, this
           only needs to be checked when we are about to output a path
           so this test can be saved till later if needed, if perfomed
           later it could also be optimized with SIMD instructions.
           Even better this check could be eliminated as we are
           outputing the tail of our traceback not the head, for any
           head state path will tend towards the optimal path with a
           probability approaching 1 in just 8 or so transitions
        */
        if(min_metric <= best_state_metric) {
          best_state_metric = min_metric;
          best_state = state;
        }
      }

      if(best_state_metric > 10000) {
        for(unsigned int state = 0; state < 8; state++)
          path_metrics[phase^1][state] -= best_state_metric;
      }
      phase ^= 1;

      return (0x3 & traceback[phase][best_state]);
    }

  } /* namespace dtv */
} /* namespace gr */


