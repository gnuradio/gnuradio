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

#ifndef INCLUDED_ldpc_R_U_mtrx_H
#define INCLUDED_ldpc_R_U_mtrx_H

#include <gnuradio/fec/fec_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      /*!
       * \brief Parity check matrix in Richardson/Urbanke format
       * \ingroup error_coding_blk
       *
       * \details
       * This class stores a matrix for use with the
       * ldpc_R_U_encoder class. It must be of the specific format
       * described by Richardson and Urbanke in Appendix A of their
       * book: Modern Coding Theory (ISBN 978-0-521-85229-6). The
       * form is:
       * \f[\left[\begin{array}{ccc} T & A & B\\ E & C & D \end{array}\right]\f]
       * This class can be used with the ldpc_bit_flip_decoder.
       *
       * To convert a parity check matrix to this format, use the
       * python functions in:
       * /lib/python2.7/dist-packages/gnuradio/fec/LDPC/Generate_LDPC_matrix.py.
       */
      class FEC_API ldpc_R_U_mtrx : public fec_mtrx
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

        // Swig needs the scope resolution operator here (for some
        // reason...?)
        gr::fec::code::fec_mtrx *d_base_ptr;

        //! Sets the submatrix variables needed for encoding
        void set_parameters_for_encoding();
        
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
        ldpc_R_U_mtrx(const std::string filename, unsigned int gap);

        //! Default constructor, should not be used
        ldpc_R_U_mtrx();

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

        /*!
         * \brief A pointer to make SWIG work
         * \details
         * For some reason, SWIG isn't accepting a pointer to this
         * child class for the make function of the
         * ldpc_bit_flip_decoder; it's expecting a pointer to the
         * base class. So, this is just a workaround for SWIG and
         * GRC.
         */
        gr::fec::code::fec_mtrx *get_base_ptr();

        /*!
         * \brief Destructor
         * \details
         * Calls the gsl_matrix_free function to free memory
         */
        virtual ~ldpc_R_U_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_ldpc_R_U_mtrx_H */