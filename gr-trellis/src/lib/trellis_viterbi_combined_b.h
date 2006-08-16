/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

// WARNING: this file is machine generated.  Edits will be over written

#ifndef INCLUDED_TRELLIS_VITERBI_COMBINED_B_H
#define INCLUDED_TRELLIS_VITERBI_COMBINED_B_H

#include "fsm.h"
#include <gr_block.h>
#include "trellis_calc_metric.h"

class trellis_viterbi_combined_b;
typedef boost::shared_ptr<trellis_viterbi_combined_b> trellis_viterbi_combined_b_sptr;

trellis_viterbi_combined_b_sptr trellis_make_viterbi_combined_b (
    const fsm &FSM,
    int D,
    const std::vector<float> &TABLE,
    int K,
    int S0,
    int SK,
    trellis_metric_type_t TYPE);


class trellis_viterbi_combined_b : public gr_block
{
  fsm d_FSM;
  int d_D;
  std::vector<float> d_TABLE;
  int d_K;
  int d_S0;
  int d_SK;
  trellis_metric_type_t d_TYPE;
  std::vector<int> d_trace;

  friend trellis_viterbi_combined_b_sptr trellis_make_viterbi_combined_b (
    const fsm &FSM,
    int D,
    const std::vector<float> &TABLE,
    int K,
    int S0,
    int SK,
    trellis_metric_type_t TYPE);


  trellis_viterbi_combined_b (
    const fsm &FSM,
    int D,
    const std::vector<float> &TABLE,
    int K,
    int S0,
    int SK,
    trellis_metric_type_t TYPE);


public:
  fsm FSM () const { return d_FSM; }
  int D () const { return d_D; }
  std::vector<float> TABLE () const { return d_TABLE; }
  int K () const { return d_K; }
  int S0 () const { return d_S0; }
  int SK () const { return d_SK; }
  trellis_metric_type_t TYPE () const { return d_TYPE; }
  //std::vector<int> trace () const { return d_trace; }
  void forecast (int noutput_items,
                 gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items);
};

#endif
