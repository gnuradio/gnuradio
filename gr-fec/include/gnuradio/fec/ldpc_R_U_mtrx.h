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

#ifndef INCLUDED_ldpc_R_U_mtrx_H
#define INCLUDED_ldpc_R_U_mtrx_H

#include <gnuradio/fec/fec_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      class FEC_API ldpc_R_U_mtrx : public fec_mtrx
      {
      private:
        // Gap (assumes matrix is in TABECD form)
        unsigned int d_gap;
        // These are the submatrices found during preprocessing
        // which are used for encoding. 
        gsl_matrix_view d_A_view;
        gsl_matrix_view d_B_view;
        gsl_matrix_view d_D_view;
        gsl_matrix_view d_E_view;
        gsl_matrix_view d_T_view;
        gsl_matrix *d_phi_inverse_ptr;

        // Set the submatrix variables needed for encoding
        void set_parameters_for_encoding();
        
      public:
        ldpc_R_U_mtrx(const std::string filename, unsigned int gap);
        // Default constructor, should not be used
        ldpc_R_U_mtrx();
        // Access the matrices needed during encoding
        const gsl_matrix *A();
        const gsl_matrix *B();
        const gsl_matrix *D();
        const gsl_matrix *E();
        const gsl_matrix *T();
        const gsl_matrix *phi_inverse();
        // Destructor
        virtual ~ldpc_R_U_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_ldpc_R_U_mtrx_H */