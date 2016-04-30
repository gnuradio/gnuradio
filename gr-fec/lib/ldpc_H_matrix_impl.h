/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ldpc_H_matrix_impl_H
#define INCLUDED_ldpc_H_matrix_impl_H

#include "fec_mtrx_impl.h"
#include <gnuradio/fec/ldpc_H_matrix.h>

namespace gr {
  namespace fec {
    namespace code {

      class ldpc_H_matrix_impl
        : public fec_mtrx_impl, public ldpc_H_matrix
      {
      private:
        // Gap (assumes matrix is in TABECD form)
        unsigned int d_gap;

        // Submatrices found during preprocessing, used for encoding
        gsl_matrix_view d_A_view;
        gsl_matrix_view d_B_view;
        gsl_matrix_view d_D_view;
        gsl_matrix_view d_E_view;
        gsl_matrix_view d_T_view;
        gsl_matrix *d_phi_inverse_ptr;

        //! Sets the submatrix variables needed for encoding
        void set_parameters_for_encoding();

        void back_solve_mod2(gsl_matrix *result,
                             const gsl_matrix *U,
                             const gsl_matrix *y) const;

        //! Access the A submatrix, needed during encoding
        const gsl_matrix *A() const;

        //! Access the B submatrix, needed during encoding
        const gsl_matrix *B() const;

        //! Access the D submatrix, needed during encoding
        const gsl_matrix *D() const;

        //! Access the E submatrix, needed during encoding
        const gsl_matrix *E() const;

        //! Access the T submatrix, needed during encoding
        const gsl_matrix *T() const;

        /*!
         * \brief Access the \f$\phi^{-1}\f$ matrix
         * \details
         * Access the matrix \f$\phi^{-1}\f$, which is needed during
         * encoding. \f$\phi\f$ is defined as:
         * \f$\phi=C-ET^{-1}A\f$.
         */
        const gsl_matrix *phi_inverse() const;

      public:
        /*!
         * \brief Constructor given alist file and gap
         * \param filename Name of an alist file to use. The alist
         *                 format is described at:
         *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
         * \param gap A property of the matrix being used. For alist
         *            files distributed with GNU Radio, this value
         *            is specified in the alist filename. The gap is
         *            found during the matrix preprocessing
         *            algorithm. It is equal to the number of rows in
         *            submatrices E, C and D.
         */
        ldpc_H_matrix_impl(const std::string filename, unsigned int gap);

        //! Encode \p inbuffer with LDPC H matrix into \p outbuffer.
        void encode(unsigned char *outbuffer,
                    const unsigned char *inbuffer) const;

        //! Decode \p inbuffer with LDPC H matrix into \p outbuffer.
        void decode(unsigned char *outbuffer,
                    const float *inbuffer,
                    unsigned int frame_size,
                    unsigned int max_iterations) const;

        //! Redefine these here as part of the public interface
        unsigned int n() const { return fec_mtrx_impl::n(); };

        //! Redefine these here as part of the public interface
        unsigned int k() const { return fec_mtrx_impl::k(); };

        gr::fec::code::fec_mtrx_sptr get_base_sptr();

        /*!
         * \brief Destructor
         * \details
         * Calls the gsl_matrix_free function to free memory
         */
        virtual ~ldpc_H_matrix_impl();
      };

    }
  }
}

#endif /* INCLUDED_ldpc_H_matrix_impl_H */
