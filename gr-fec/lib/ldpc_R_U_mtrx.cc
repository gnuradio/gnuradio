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

#include <gnuradio/fec/ldpc_R_U_mtrx.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      ldpc_R_U_mtrx::ldpc_R_U_mtrx(const std::string filename, unsigned int gap) 
      {
        d_H_ptr = read_matrix_from_file(filename);
        d_gap = gap;

        // Length of codeword = # of columns
        d_n = d_num_cols;

        // Length of information word = (# of columns) - (# of rows)
        d_k = d_num_cols - d_num_rows;

        set_parameters_for_encoding();

      } // Constructor

      // Default constructor, should not be used
      ldpc_R_U_mtrx::ldpc_R_U_mtrx()
      {
        std::cout << "Error in ldpc_R_U_mtrx(): Default "
                  << "constructor called.\nMust provide filename for"
                  << " parity check matrix. \n\n";
        exit(1);
      } // Default constructor

      const gsl_matrix*
      ldpc_R_U_mtrx::A()
      {
        const gsl_matrix *A_ptr = &d_A_view.matrix;
        return A_ptr; 
      }

      const gsl_matrix*
      ldpc_R_U_mtrx::B()
      {
        const gsl_matrix *B_ptr = &d_B_view.matrix;
        return B_ptr; 
      }

      const gsl_matrix*
      ldpc_R_U_mtrx::D()
      {
        const gsl_matrix *D_ptr = &d_D_view.matrix;
        return D_ptr; 
      }

      const gsl_matrix*
      ldpc_R_U_mtrx::E()
      {
        const gsl_matrix *E_ptr = &d_E_view.matrix;
        return E_ptr; 
      }

      const gsl_matrix*
      ldpc_R_U_mtrx::T()
      {
        const gsl_matrix *T_ptr = &d_T_view.matrix;
        return T_ptr; 
      }

      const gsl_matrix*
      ldpc_R_U_mtrx::phi_inverse()
      {
        const gsl_matrix *phi_inverse_ptr = d_phi_inverse_ptr;
        return phi_inverse_ptr; 
      }

      void 
      ldpc_R_U_mtrx::set_parameters_for_encoding() 
      {

        // This function defines all of the submatrices that will be
        // needed during encoding. 

        unsigned int t = d_num_rows - d_gap; 
        
        // T submatrix
        d_T_view = gsl_matrix_submatrix(d_H_ptr, 0, 0, t, t);

        gsl_matrix *d_T_inverse_ptr;
        try {
          d_T_inverse_ptr = calc_inverse_mod2(&d_T_view.matrix);
        }
        catch (char const *exceptionString) {
          std::cout << "Error in set_parameters_for_encoding while "
                    << "looking for inverse T matrix: " 
                    << exceptionString
                    << "Tip: verify that the correct gap is being "
                    << "specified for this alist file.\n";

          exit(1);
        }

        // E submatrix
        d_E_view = gsl_matrix_submatrix(d_H_ptr, t, 0, d_gap,
                                        d_n-d_k-d_gap);

        // A submatrix
        d_A_view = gsl_matrix_submatrix(d_H_ptr, 0, t, t, d_gap);

        // C submatrix (used to find phi but not during encoding)
        gsl_matrix_view C_view = gsl_matrix_submatrix(d_H_ptr, t, t,
                                                      d_gap, d_gap);

        // These are just temporary matrices used to find phi.
        gsl_matrix *temp1 = mult_matrices_mod2(&d_E_view.matrix,
                                               d_T_inverse_ptr);
        gsl_matrix *temp2 = mult_matrices_mod2(temp1, 
                                               &d_A_view.matrix);

        // Solve for phi.
        gsl_matrix *phi = add_matrices_mod2(&C_view.matrix, temp2);

        // If phi has at least one nonzero entry, try for inverse.
        if (gsl_matrix_max(phi)) {
          try {
            gsl_matrix *inverse_phi = calc_inverse_mod2(phi);
              
            // At this point, an inverse was found. 
            d_phi_inverse_ptr = inverse_phi;

          }
          catch (char const *exceptionString) {

            std::cout << "Error in set_parameters_for_encoding while"
                      << " finding inverse_phi: " << exceptionString
                      << "Tip: verify that the correct gap is being "
                      << "specified for this alist file.\n";      
            exit(1);
          }
        }

        // B submatrix
        d_B_view = gsl_matrix_submatrix(d_H_ptr, 0, t + d_gap, t,
                                        d_n-d_gap-t);

        // D submatrix
        d_D_view = gsl_matrix_submatrix(d_H_ptr, t, t + d_gap, d_gap,
                                        d_n-d_gap-t);

        // Free memory
        gsl_matrix_free(temp1);
        gsl_matrix_free(temp2);
        gsl_matrix_free(phi);
        gsl_matrix_free(d_T_inverse_ptr);

      }

      ldpc_R_U_mtrx::~ldpc_R_U_mtrx()
      {
        // Call the gsl_matrix_free function to free memory.
        gsl_matrix_free (d_H_ptr);
        gsl_matrix_free (d_phi_inverse_ptr);
      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */