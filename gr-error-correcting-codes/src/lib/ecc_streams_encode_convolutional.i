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

GR_SWIG_BLOCK_MAGIC(ecc,streams_encode_convolutional);

// Rename second constructor
%rename(streams_encode_convolutional_feedback) ecc_make_streams_encode_convolutional_feedback;

// support vectors of these ... for use in the convolutional decoder
// as well as Turbo Codes (both encoder and decoder).

namespace std {
  %template(x_vector_ecc_streams_encode_convolutional_sptr) vector<ecc_streams_encode_convolutional_sptr>;
};

ecc_streams_encode_convolutional_sptr
ecc_make_streams_encode_convolutional
(int block_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

ecc_streams_encode_convolutional_sptr
ecc_make_streams_encode_convolutional_feedback
(int block_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

class ecc_streams_encode_convolutional : public gr_block
{
private:
  ecc_streams_encode_convolutional
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  ecc_streams_encode_convolutional
  (int block_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   const std::vector<int> &code_feedback,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);
};
