/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H
#define INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H

#include <gnuradio/fec/ldpc_par_mtrx_encoder.h>

namespace gr {
  namespace fec {
    namespace code {

      class ldpc_par_mtrx_encoder_impl : public ldpc_par_mtrx_encoder
      {
      private:
        // plug into the generic fec api
        void generic_work(void *inbuffer, void *outbuffer);

        // Number of bits in the frame to be encoded
        unsigned int d_frame_size;

        // Number of output bits after coding
        int d_output_size;

        // Rate of the code, n/k
        double d_rate;

        // LDPC parity check matrix object
        code::ldpc_H_matrix::sptr d_H;

      public:
        ldpc_par_mtrx_encoder_impl(const code::ldpc_H_matrix::sptr H_obj);
        ~ldpc_par_mtrx_encoder_impl();

        double rate();
        bool set_frame_size(unsigned int frame_size);
        int get_output_size();
        int get_input_size();
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_LDPC_PAR_MTRX_ENCODER_IMPL_H */
