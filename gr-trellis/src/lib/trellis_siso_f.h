/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_SISO_F_H
#define INCLUDED_TRELLIS_SISO_F_H

#include <trellis_api.h>
#include "fsm.h"
#include "siso_type.h"
#include "core_algorithms.h"
#include <gr_block.h>

class trellis_siso_f;
typedef boost::shared_ptr<trellis_siso_f> trellis_siso_f_sptr;

TRELLIS_API trellis_siso_f_sptr trellis_make_siso_f (
    const fsm &FSM, 	// underlying FSM
    int K,		// block size in trellis steps
    int S0,		// initial state (put -1 if not specified)
    int SK,		// final state (put -1 if not specified)
    bool POSTI,		// true if you want a-posteriori info about the input symbols to be mux-ed in the output
    bool POSTO,		// true if you want a-posteriori info about the output symbols to be mux-ed in the output
    trellis_siso_type_t d_SISO_TYPE // perform "min-sum" or "sum-product" combining 
);


/*!
 *  \ingroup coding_blk
 */
class TRELLIS_API trellis_siso_f : public gr_block
{
  fsm d_FSM;
  int d_K;
  int d_S0;
  int d_SK;
  bool d_POSTI;
  bool d_POSTO;
  trellis_siso_type_t d_SISO_TYPE;
  //std::vector<float> d_alpha;
  //std::vector<float> d_beta;

  friend TRELLIS_API trellis_siso_f_sptr trellis_make_siso_f (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    bool POSTI,
    bool POSTO,
    trellis_siso_type_t d_SISO_TYPE);


  trellis_siso_f (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    bool POSTI,
    bool POSTO,
    trellis_siso_type_t d_SISO_TYPE);


public:
  fsm FSM () const { return d_FSM; }
  int K () const { return d_K; }
  int S0 () const { return d_S0; }
  int SK () const { return d_SK; }
  bool POSTI () const { return d_POSTI; }
  bool POSTO () const { return d_POSTO; }
  trellis_siso_type_t SISO_TYPE () const { return d_SISO_TYPE; }
  void forecast (int noutput_items,
                 gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items);
};

#endif
