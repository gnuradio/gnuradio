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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

GR_SWIG_BLOCK_MAGIC(ecc,metrics_decode_viterbi_full_block);

// corrected "class NAME" statement, which uses slightly different NAMEs

typedef boost::shared_ptr<ecc_metrics_decode_viterbi_full_block>
ecc_metrics_decode_viterbi_full_block_feedback_sptr;

LOCAL_GR_SWIG_BLOCK_MAGIC(ecc,metrics_decode_viterbi_full_block_feedback);

ecc_metrics_decode_viterbi_full_block_sptr
ecc_make_metrics_decode_viterbi_full_block
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination,
 int start_memory_state,
 int end_memory_state);

ecc_metrics_decode_viterbi_full_block_feedback_sptr
ecc_make_metrics_decode_viterbi_full_block_feedback
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination,
 int start_memory_state,
 int end_memory_state);

class ecc_metrics_decode_viterbi_full_block : public gr_block
{
  ecc_metrics_decode_viterbi_full_block
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  ecc_metrics_decode_viterbi_full_block
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   const std::vector<int> &code_feedback,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);
};
