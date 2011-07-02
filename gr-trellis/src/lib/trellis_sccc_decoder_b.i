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

// WARNING: this file is machine generated.  Edits will be over written

GR_SWIG_BLOCK_MAGIC(trellis,sccc_decoder_b);

trellis_sccc_decoder_b_sptr trellis_make_sccc_decoder_b (
  const fsm &FSMo, int STo0, int SToK,
  const fsm &FSMi, int STi0, int STiK,
  const interleaver &INTERLEAVER,
  int blocklength,
  int repetitions,
  trellis_siso_type_t SISO_TYPE
);


class trellis_sccc_decoder_b : public gr_block
{
private:
  trellis_sccc_decoder_b (
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
};
