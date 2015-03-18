/* -*- c++ -*- */
/* 
 * Copyright 2013-2014 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation; either version 3, or (at your 
 * option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ldpc_gen_mtrx_encoder_impl.h>
#include <sstream>

namespace gr {
  namespace fec {
    namespace code {
      generic_encoder::sptr
      ldpc_gen_mtrx_encoder::make(const ldpc_HorG_mtrx *M_obj)
      {
        return generic_encoder::sptr
          (new ldpc_gen_mtrx_encoder_impl(M_obj));
      }

      ldpc_gen_mtrx_encoder_impl::ldpc_gen_mtrx_encoder_impl(const ldpc_HorG_mtrx *M_obj) 
        : generic_encoder("ldpc_gen_mtrx_encoder")
      {
        // Generator matrix to use for encoding
        d_M = M_obj;
        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_M->k());
      }

      ldpc_gen_mtrx_encoder_impl::~ldpc_gen_mtrx_encoder_impl()
      {
      }

      int
      ldpc_gen_mtrx_encoder_impl::get_output_size()
      {
        return d_M->n();
      }

      int
      ldpc_gen_mtrx_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      ldpc_gen_mtrx_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        // TODO add some bounds check here? The frame size is
        // constant and specified by the size of the parity check
        // matrix used for encoding.
        d_frame_size = frame_size;

        return ret;        
      }

      double
      ldpc_gen_mtrx_encoder_impl::rate()
      {
        return (d_M->n())/static_cast<double>(d_frame_size);
      }

      void
      ldpc_gen_mtrx_encoder_impl::generic_work(void *inbuffer,
                                               void *outbuffer)
      {
        // Populate the information word
        const unsigned char *in = (const unsigned char *)inbuffer;
        unsigned int index, k = d_M->k(), n = d_M->n();
        gsl_matrix *s = gsl_matrix_alloc(k, 1);
        for (index = 0; index < k; index++) {
          double value = static_cast<double>(in[index]);
          gsl_matrix_set(s, index, 0, value);
        }

        // Simple matrix multiplication to get codeword
        gsl_matrix *codeword;
        codeword = d_M->mult_matrices_mod2(d_M->G_transpose(), s);

        // Output
        unsigned char *out = (unsigned char*)outbuffer;
        for (index = 0; index < n; index++) {
          out[index] = gsl_matrix_get(codeword, index, 0);
        }

        // Free memory
        gsl_matrix_free(codeword);


      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */