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
#include "interleaver.h"
#include <gr_block.h>
#include <vector>
#include "siso_type.h"

class @NAME@;
typedef boost::shared_ptr<@NAME@> @SPTR_NAME@;

TRELLIS_API @SPTR_NAME@ trellis_make_@BASE_NAME@ (
  const fsm &FSMo, int STo0, int SToK,
  const fsm &FSMi, int STi0, int STiK,
  const interleaver &INTERLEAVER,
  int blocklength,
  int repetitions,
  trellis_siso_type_t SISO_TYPE // perform "min-sum" or "sum-product" combining
);


/*!
 *  \ingroup trellis_coding_blk
 */
class TRELLIS_API @NAME@ : public gr_block
{
  fsm d_FSMo;
  int d_STo0;
  int d_SToK;
  fsm d_FSMi;
  int d_STi0;
  int d_STiK;
  interleaver d_INTERLEAVER;
  int d_blocklength;
  int d_repetitions;
  trellis_siso_type_t d_SISO_TYPE;
  std::vector<float> d_buffer;

  friend TRELLIS_API @SPTR_NAME@ trellis_make_@BASE_NAME@ (
    const fsm &FSMo, int STo0, int SToK,
    const fsm &FSMi, int STi0, int STiK,
    const interleaver &INTERLEAVER,
    int blocklength,
    int repetitions,
    trellis_siso_type_t SISO_TYPE
  );

  @NAME@ (
    const fsm &FSMo, int STo0, int SToK,
    const fsm &FSMi, int STi0, int STiK,
    const interleaver &INTERLEAVER,
    int blocklength,
    int repetitions,
    trellis_siso_type_t SISO_TYPE
  );

public:
  fsm FSMo () const { return d_FSMo; }
  fsm FSMi () const { return d_FSMi; }
  int STo0 () const { return d_STo0; }
  int SToK () const { return d_SToK; }
  int STi0 () const { return d_STi0; }
  int STiK () const { return d_STiK; }
  interleaver INTERLEAVER () const { return d_INTERLEAVER; }
  int blocklength () const { return d_blocklength; }
  int repetitions () const { return d_repetitions; }
  trellis_siso_type_t SISO_TYPE () const { return d_SISO_TYPE; }

  void forecast (int noutput_items,
                 gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
                    gr_vector_int &ninput_items,
                    gr_vector_const_void_star &input_items,
                    gr_vector_void_star &output_items);
};

#endif
