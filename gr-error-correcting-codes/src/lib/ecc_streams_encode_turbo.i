/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(ecc,streams_encode_turbo);

#include <ecc_streams_encode_convolutional.h>

ecc_streams_encode_turbo_sptr
ecc_make_streams_encode_turbo
(int n_code_inputs,
 int n_code_outputs,
 const std::vector<ecc_streams_encode_convolutional_sptr> &encoders,
 const std::vector<size_t> &interleavers);

class ecc_streams_encode_turbo : public gr_block
{
  ecc_streams_encode_turbo
  (int frame_size_bits,
   int n_code_inputs,
   const std::vector<ecc_streams_encode_convolutional_sptr> &encoders,
   const std::vector<size_t> &interleavers);
};
