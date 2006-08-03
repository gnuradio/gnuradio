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

#ifndef INCLUDED_DECODER_VITERBI_FULL_BLOCK_IC1_H
#define INCLUDED_DECODER_VITERBI_FULL_BLOCK_IC1_H

#include "decoder_viterbi_full_block.h"

class decoder_viterbi_full_block_i1_ic1 : public decoder_viterbi_full_block
{
/*!
 * class decoder_viterbi_full_block_i1_ic1 :
 *     public decoder_viterbi_full_block
 *
 * Decode the incoming metrics using a Viterbi-style decoder, doing
 *     full trellis block decoding before putting out any bits
 *
 * input is "i1": one stream per input (as defined by the
 *     code-output), with one metric per input item.
 *
 * output is "ic1": streams of char, one stream per output (as defined
 *     by the code-input), using only the right-most justified (LSB?)
 *     bit as the single output bit per output item.
 *
 * The rest of the options are outlined in the inherited classes'
 *     header files.
 */

public:
  inline decoder_viterbi_full_block_i1_ic1
  (int sample_precision,
   encoder_convolutional* l_encoder)
    : decoder_viterbi_full_block (sample_precision, l_encoder) {};

  virtual ~decoder_viterbi_full_block_i1_ic1 () {};

  virtual size_t compute_n_input_metrics (size_t n_output_bits);
  virtual size_t compute_n_output_bits (size_t n_input_metrics);

protected:
  virtual void increment_input_indices (bool while_decoding);
  virtual void output_bit (char t_out_bit, char** out_buf,
			   size_t t_output_stream);
  virtual void increment_output_indices (bool while_decoding);
};

#endif /* INCLUDED_DECODER_VITERBI_FULL_BLOCK_I1_IC1_H */
