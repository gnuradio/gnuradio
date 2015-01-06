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
      ldpc_R_U_encoder::make(ldpc_R_U_mtrx *H_obj)
      {
        return generic_encoder::sptr
          (new ldpc_R_U_encoder_impl(H_obj));
      }

      ldpc_R_U_encoder_impl::ldpc_R_U_encoder_impl(ldpc_R_U_mtrx *H_obj) 
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

      gsl_matrix*
      ldpc_R_U_encoder_impl::back_solve_mod2(const gsl_matrix *U,
                                             const gsl_matrix *y)
      {
        // Exploit the fact that the matrix T is upper triangular and
        // sparse. In the steps to find p1 and p2, back solve rather 
        // than do matrix multiplication to reduce number of
        // operations required. 

        // Form is Ux = y where U is upper triangular and y is column
        // vector. Solve for x.

        // Allocate memory for the result
        int num_rows   = (*U).size1;
        int num_cols_U = (*U).size2;
        gsl_matrix *x  = gsl_matrix_alloc(num_rows,1);

        // Back solve
        for (int i = num_rows-1; i >= 0; i--) {
          // x[i] = y[i]
          gsl_matrix_set(x, i, 0, gsl_matrix_get(y, i, 0));

          int j;
          for (j = i+1; j < num_cols_U; j++) {
            int U_i_j = gsl_matrix_get(U, i, j);
            int x_i   = gsl_matrix_get(x, i, 0);
            int x_j   = gsl_matrix_get(x, j, 0);
            int temp1 = (U_i_j * x_j) % 2;
            int temp2 = (x_i + temp1) % 2;
            gsl_matrix_set(x, i, 0, temp2);
          }
          // Perform x[i] /= U[i,i], GF(2) operations
          int U_i_i = gsl_matrix_get(U, i, i);
          int x_i   = gsl_matrix_get(x, i, 0);
          if      (x_i==0 && U_i_i==1)
            gsl_matrix_set(x, i, 0, 0);
          else if (x_i==0 && U_i_i==0)
            gsl_matrix_set(x, i, 0, 0);
          else if (x_i==1 && U_i_i==1)
            gsl_matrix_set(x, i, 0, 1);
          else if (x_i==1 && U_i_i==0)
            std::cout << "Error in "
                      << " ldpc_R_U_encoder_impl::back_solve_mod2,"
                      << " division not defined.\n";  
          else
            std::cout << "Error in ldpc_R_U_encoder_impl::back_solve_mod2\n";  
        }
        return x;
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

        // Solve for p2 (parity part). By using back substitution,
        // the overall complexity of determining p2 is O(n + g^2).
        gsl_matrix *temp1 = d_H->mult_matrices_mod2(d_H->B(), s);
        gsl_matrix *temp2 = back_solve_mod2(d_H->T(),temp1);
        gsl_matrix *temp3 = d_H->mult_matrices_mod2(d_H->E(), temp2);
        gsl_matrix *temp4 = d_H->mult_matrices_mod2(d_H->D(), s);
        gsl_matrix *temp5 = d_H->add_matrices_mod2(temp4, temp3);
        gsl_matrix *p2    = d_H->mult_matrices_mod2(
                                         d_H->phi_inverse(), temp5);

        // Solve for p1 (parity part). By using back substitution,
        // the overall complexity of determining p1 is O(n).
        gsl_matrix *temp6 = d_H->mult_matrices_mod2(d_H->A(), p2);
        gsl_matrix *temp7 = d_H->add_matrices_mod2(temp6, temp1);
        gsl_matrix *p1 =  back_solve_mod2(d_H->T(),temp7);

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