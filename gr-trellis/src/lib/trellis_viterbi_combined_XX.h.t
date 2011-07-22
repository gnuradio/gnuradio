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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <trellis_api.h>
#include "fsm.h"
#include <gr_block.h>
#include "calc_metric.h"
#include "core_algorithms.h"

class @NAME@;
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

TRELLIS_API @SPTR_NAME@ trellis_make_@BASE_NAME@ (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<@I_TYPE@> &TABLE,
    trellis_metric_type_t TYPE);


/*!
 *  \ingroup coding_blk
 */
class TRELLIS_API @NAME@ : public gr_block
{
  fsm d_FSM;
  int d_K;
  int d_S0;
  int d_SK;
  int d_D;
  std::vector<@I_TYPE@> d_TABLE;
  trellis_metric_type_t d_TYPE;
  //std::vector<int> d_trace;

  friend TRELLIS_API @SPTR_NAME@ trellis_make_@BASE_NAME@ (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<@I_TYPE@> &TABLE,
    trellis_metric_type_t TYPE);


  @NAME@ (
    const fsm &FSM,
    int K,
    int S0,
    int SK,
    int D,
    const std::vector<@I_TYPE@> &TABLE,
    trellis_metric_type_t TYPE);


public:
  fsm FSM () const { return d_FSM; }
  int K () const { return d_K; }
  int S0 () const { return d_S0; }
  int SK () const { return d_SK; }
  int D () const { return d_D; }
  std::vector<@I_TYPE@> TABLE () const { return d_TABLE; }
  trellis_metric_type_t TYPE () const { return d_TYPE; }
  //std::vector<int> trace () const { return d_trace; }
  void set_TABLE (const std::vector<@I_TYPE@> &table);
  void forecast (int noutput_items,
                 gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items);
};

#endif
