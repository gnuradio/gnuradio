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

#include <ldpc_R_U_encoder_impl.h>
#include <sstream>

namespace gr {
  namespace fec {
    namespace code {
      generic_encoder::sptr
      ldpc_R_U_encoder::make(ldpc_par_chk_mtrx *H_obj)
      {
        return generic_encoder::sptr
          (new ldpc_R_U_encoder_impl(H_obj));
      }

      ldpc_R_U_encoder_impl::ldpc_R_U_encoder_impl(ldpc_par_chk_mtrx *H_obj) 
        : generic_encoder("ldpc_R_U_encoder")
      {
        // LDPC parity check matrix to use for encoding
        d_H = H_obj;
        // Set frame size to k, the # of bits in the information word
        // All buffers and settings will be based on this value.
        set_frame_size(d_H->k());
      }

      ldpc_R_U_encoder_impl::~ldpc_R_U_encoder_impl()
      {
      }

      int
      ldpc_R_U_encoder_impl::get_output_size()
      {
        return d_H->n();
      }

      int
      ldpc_R_U_encoder_impl::get_input_size()
      {
        return d_frame_size;
      }

      bool
      ldpc_R_U_encoder_impl::set_frame_size(unsigned int frame_size)
      {
        bool ret = true;
        // TODO add some bounds check here? The frame size is
        // constant and specified by the size of the parity check
        // matrix used for encoding.
        d_frame_size = frame_size;

        return ret;        
      }

      double
      ldpc_R_U_encoder_impl::rate()
      {
        return (d_H->n())/static_cast<double>(d_frame_size);
      }

      void
      ldpc_R_U_encoder_impl::generic_work(void *inbuffer,
                                          void *outbuffer)
      {
        // Populate the information word
        const unsigned char *in = (const unsigned char *)inbuffer;
        unsigned int index, k = d_H->k();
        gsl_matrix *s = gsl_matrix_alloc(k, 1);
        for (index = 0; index < k; index++) {
          double value = static_cast<double>(in[index]);
          gsl_matrix_set(s, index, 0, value);
        }

        // Solve for p2 and p1 (parity parts)
        gsl_matrix *temp1 = d_H->mult_matrices_mod2(d_H->B(), s);
        gsl_matrix *temp2 = d_H->mult_matrices_mod2(
                                         d_H->T_inverse(), temp1);
        gsl_matrix *temp3 = d_H->mult_matrices_mod2(d_H->E(), temp2);
        gsl_matrix *temp4 = d_H->mult_matrices_mod2(d_H->D(), s);
        gsl_matrix *temp5 = d_H->add_matrices_mod2(temp4, temp3);
        gsl_matrix *p2    = d_H->mult_matrices_mod2(
                                         d_H->phi_inverse(), temp5);
        gsl_matrix *temp6 = d_H->mult_matrices_mod2(d_H->A(), p2);
        gsl_matrix *temp7 = d_H->add_matrices_mod2(temp6, temp1);
        gsl_matrix *p1    = d_H->mult_matrices_mod2(
                                         d_H->T_inverse(), temp7);

        // Populate the codeword to be output
        unsigned int p1_length = (*p1).size1;
        unsigned int p2_length = (*p2).size1;
        unsigned char *out = (unsigned char*)outbuffer;
        for (index = 0; index < p1_length; index++) {
          int value = gsl_matrix_get(p1, index, 0);
          out[index] = value;
        }
        for (index = 0; index < p2_length; index++) {
          int value = gsl_matrix_get(p2, index, 0);
          out[p1_length+index] = value;
        }
        for (index = 0; index < k; index++) {
          int value = gsl_matrix_get(s, index, 0);
          out[p1_length+p2_length+index] = value;
        }

        // Free memory
        gsl_matrix_free(temp1);
        gsl_matrix_free(temp2);
        gsl_matrix_free(temp3);
        gsl_matrix_free(temp4);
        gsl_matrix_free(temp5);
        gsl_matrix_free(temp6);
        gsl_matrix_free(temp7);
        gsl_matrix_free(p1);
        gsl_matrix_free(p2);
        gsl_matrix_free(s);

      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */