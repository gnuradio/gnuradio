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

#ifndef INCLUDED_ECC_METRICS_DECODE_VITERBI_FULL_BLOCK_H
#define INCLUDED_ECC_METRICS_DECODE_VITERBI_FULL_BLOCK_H

#include <gr_block.h>
#include <libecc/decoder_viterbi_full_block.h>
#include <libecc/encoder_convolutional.h>

class ecc_metrics_decode_viterbi_full_block;

typedef boost::shared_ptr<ecc_metrics_decode_viterbi_full_block>
ecc_metrics_decode_viterbi_full_block_sptr,
ecc_metrics_decode_viterbi_full_block_feedback_sptr;

ecc_metrics_decode_viterbi_full_block_sptr
ecc_make_metrics_decode_viterbi_full_block
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int>& code_generator,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

ecc_metrics_decode_viterbi_full_block_feedback_sptr
ecc_make_metrics_decode_viterbi_full_block_feedback
(int sample_precision,
 int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int>& code_generator,
 const std::vector<int>& code_feedback,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

/*!
 * \brief Decode the incoming streams using a Viterbi-style decoder,
 *     full trellis block decoding.
 *
 * input: symbol metrics data.
 *
 * output: stream(s) of char, single bit stored in the LSB.
 */

class ecc_metrics_decode_viterbi_full_block : public gr_block
{
protected:
  friend ecc_metrics_decode_viterbi_full_block_sptr
  ecc_make_metrics_decode_viterbi_full_block
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generator,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  friend ecc_metrics_decode_viterbi_full_block_feedback_sptr
  ecc_make_metrics_decode_viterbi_full_block_feedback
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generator,
   const std::vector<int>& code_feedback,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);
/*
 * frame_size_bits: if == 0, then do streaming decoding (infinite
 *     trellis); otherwise this is the block size to decode before
 *     terminating the trellis.
 *
 * code_generator: vector of integers (32 bit) representing the code
 *     to be implemented in octal form.  E.g. "04" in binary is "100",
 *     which would be "D^2" for code generation.  "06" == 110b == "D^2 + D"
 *  ==> The vector is listed in order for each input stream, so if there
 *     are 2 input streams (I1, I2) [specified in "n_code_inputs"]
 *     and 2 output streams (O1, O2) [specified in "n_code_outputs"],
 *     then the vector would be the code generator for:
 *       [I1->O1, I1->O2, I2->O1, I2->O2]
 *     with each element being the octal representation of the code.
 *
 * do_termination is valid only if frame_size_bits != 0, and defines
 *     whether or not to to trellis termination.
 *
 * do_mux_input: is true, then the one input stream will be
 *     interpreted as the multiplexing all of the input bits.  For
 *     example for a 3 input decoder, the input stream would be
 *     [I1[k], I2[k], I3[k], I1[k+1], I2[k+1], ...], where "k" is the
 *     input sample time, and "I1" through "I3" are the 3 input
 *     streams.
 *
 * n_code_inputs: the number of decoder-input (encoder-output)
 *     streams, no matter if separate or mux'ed
 *
 * n_code_outputs: the number of decoder-output (encoder-input) streams
 */

  ecc_metrics_decode_viterbi_full_block
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generator,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  ecc_metrics_decode_viterbi_full_block
  (int sample_precision,
   int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int>& code_generator,
   const std::vector<int>& code_feedback,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  void setup_io_signatures (int sample_precision,
			    int n_code_inputs,
			    int n_code_outputs);

  int d_n_code_inputs, d_n_code_outputs;
  decoder_viterbi_full_block* d_decoder;
  encoder_convolutional* d_encoder;
  code_input_ptr d_in_buf;
  code_output_ptr d_out_buf;

public:
  ~ecc_metrics_decode_viterbi_full_block ();

  virtual void forecast (int noutput_items,
			 gr_vector_int& ninput_items_required);

  virtual int general_work (int noutput_items,
			    gr_vector_int& ninput_items,
			    gr_vector_const_void_star& input_items,
			    gr_vector_void_star& output_items);
};

#endif /* INCLUDED_ECC_METRICS_DECODE_VITERBI_FULL_BLOCK_H */
