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

#ifndef INCLUDED_ECC_STREAMS_ENCODE_CONVOLUTIONAL_H
#define INCLUDED_ECC_STREAMS_ENCODE_CONVOLUTIONAL_H

#include <gr_block.h>
#include <libecc/encoder_convolutional.h>

/*!
 * \brief Encode the incoming streams using a convolutional encoder
 *
 * input: streams of char, one stream per input as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single input bit per input item.
 *
 * output: streams of char, one stream per output as defined by the
 *     instantiated code, using only the right-most justified bit as
 *     the single output bit per output item.
 *
 * frame_size_bits: if == 0, then do streaming encoding ("infinite"
 *     trellis); otherwise this is the frame size in bits to encode
 *     before terminating the trellis.  This value -does not- include
 *     any termination bits.
 *
 * n_code_inputs:
 * n_code_outputs:
 * code_generator: vector of integers (32 bit) representing the code
 *     to be implemented.  E.g. "4" in binary is "100", which would be
 *     "D^2" for code generation.  "6" == 110b == "D^2 + D"
 *  ==> The vector is listed in order for each output stream, so if there
 *     are 2 input streams (I1, I2) [specified in "n_code_inputs"]
 *     and 2 output streams (O1, O2) [specified in "n_code_outputs"],
 *     then the vector would be the code generator for:
 *       [I1->O1, I2->O1, I1->O2, I2->O2]
 *     with each element being an integer representation of the code.
 *
 * do_termination: valid only if frame_size_bits != 0, and defines
 *     whether or not to use trellis termination.  Default is to use
 *     termination when doing block coding.
 *
 * start_memory_state: when starting a new block, the starting memory
 *     state to begin encoding; there will be a helper function to
 *     assist in creating this value for a given set of inputs;
 *     default is the "all zero" state.
 * 
 * end_memory_state: when terminating a block, the ending memory
 *     state to stop encoding; there will be a helper function to
 *     assist in creating this value for a given set of inputs;
 *     default is the "all zero" state.
 */

class ecc_streams_encode_convolutional;
typedef boost::shared_ptr<ecc_streams_encode_convolutional>
  ecc_streams_encode_convolutional_sptr;

ecc_streams_encode_convolutional_sptr
ecc_make_streams_encode_convolutional
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

ecc_streams_encode_convolutional_sptr
ecc_make_streams_encode_convolutional_feedback
(int frame_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const std::vector<int> &code_generator,
 const std::vector<int> &code_feedback,
 bool do_termination = true,
 int start_memory_state = 0,
 int end_memory_state = 0);

class ecc_streams_encode_convolutional : public gr_block
{
protected:
  friend ecc_streams_encode_convolutional_sptr
  ecc_make_streams_encode_convolutional
  (int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  friend ecc_streams_encode_convolutional_sptr
  ecc_make_streams_encode_convolutional_feedback
  (int frame_size_bits,
   int n_code_inputs,
   int n_code_outputs,
   const std::vector<int> &code_generator,
   const std::vector<int> &code_feedback,
   bool do_termination,
   int start_memory_state,
   int end_memory_state);

  ecc_streams_encode_convolutional (int frame_size_bits,
				   int n_code_inputs,
				   int n_code_outputs,
				   const std::vector<int> &code_generator,
				   bool do_termination,
				   int start_memory_state,
				   int end_memory_state);

  ecc_streams_encode_convolutional (int frame_size_bits,
				   int n_code_inputs,
				   int n_code_outputs,
				   const std::vector<int> &code_generator,
				   const std::vector<int> &code_feedback,
				   bool do_termination,
				   int start_memory_state,
				   int end_memory_state);

  void setup_io_signatures (int n_code_inputs, int n_code_outputs);

  encoder_convolutional* d_encoder;
  code_input_ptr d_in_buf;
  code_output_ptr d_out_buf;

public:
  ~ecc_streams_encode_convolutional ();

  inline encoder_convolutional* encoder () {return (d_encoder);};

  virtual void forecast (int noutput_items,
			 gr_vector_int &ninput_items_required);

  virtual int general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_ECC_STREAMS_ENCODE_CONVOLUTIONAL_H */
