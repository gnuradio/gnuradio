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

#include <math.h>
#include <gnuradio/atsc/single_viterbi_impl.h>
#include <iostream>

using std::cerr;
using std::cout;

const float atsci_single_viterbi::was_sent[32] = {
  -7,-3,-7,-3,-7,-3,-7,-3,
  -5,-1,-5,-1,-5,-1,-5,-1,
  1,5,1,5,1,5,1,5,
  3,7,3,7,3,7,3,7
};

const int atsci_single_viterbi::transition_table[32] = {
  0,2,4,6,
  2,0,6,4,
  1,3,5,7,
  3,1,7,5,
  4,6,0,2,
  6,4,2,0,
  5,7,1,3,
  7,5,3,1
};

void
atsci_single_viterbi::reset()
{
  for (unsigned int i = 0; i<2; i++)
    for (unsigned int j = 0; j<8; j++) {
      path_metrics[i][j] = 0;
      traceback[i][j] = 0;
    }
  phase = 0;
}

atsci_single_viterbi::atsci_single_viterbi()
{
  reset();
}

char
atsci_single_viterbi::decode(float input)
{
  for (unsigned int next_state = 0; next_state < 8; next_state++) {
    unsigned int index = next_state << 2;
    int min_metric_symb = 0;
    float min_metric = fabs(input - was_sent[index + 0]) +
      path_metrics[phase][transition_table[index + 0]];

    for (unsigned int symbol_sent = 1; symbol_sent < 4; symbol_sent++)
      if( (fabs(input-was_sent[index+symbol_sent]) +
	   path_metrics[phase][transition_table[index+symbol_sent]])
	  < min_metric) {
	min_metric = fabs(input-was_sent[index+symbol_sent]) +
	  path_metrics[phase][transition_table[index+symbol_sent]];
	min_metric_symb = symbol_sent;
      }

    path_metrics[phase^1][next_state] = min_metric;
    traceback[phase^1][next_state] = (((unsigned long long)min_metric_symb) << 62) |
      (traceback[phase][transition_table[index+min_metric_symb]] >> 2);
  }
  unsigned int best_state = 0;
  float best_state_metric = path_metrics[phase^1][0];
  for (unsigned int state = 1; state < 8; state++)
    if(path_metrics[phase^1][state] < best_state_metric) {
      best_state = state;
      best_state_metric = path_metrics[phase^1][state];
    }
  if(best_state_metric > 10000) {
    for(unsigned int state = 0; state < 8; state++)
      path_metrics[phase^1][state] -= best_state_metric;
    // cerr << "Resetting Path Metrics from " << best_state_metric << " to 0\n";
  }
  phase ^= 1;
  return (0x3 & traceback[phase][best_state]);
}
