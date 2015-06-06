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

#ifndef INCLUDED_fec_mtrx_H
#define INCLUDED_fec_mtrx_H

#include <gnuradio/fec/api.h>
#include <string>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Base class for FEC matrix objects.
       *
       *
       * \ingroup error_coding_blk
       *
       * \details
       *
       * Base class of ldpc_HorG_mtrx and ldpc_R_U_mtrx classes.
       * The child objects can be either generator matrices or
       * parity check matrices. This base class can be provided to
       * the decoder ldpc_bit_flip_decoder, whereas the encoder
       * classes ldpc_gen_mtrx_encoder and ldpc_R_U_encoder will
       * not accept this base class; they require one of the child
       * classes.
       */
      class FEC_API fec_mtrx
      {
      protected:
        //! Constructor
        fec_mtrx();

        //! Codeword length n
        unsigned int d_n;

        //! Information word length k
        unsigned int d_k;

        //! Number of rows in the matrix read in from alist file
        unsigned int d_num_rows;

        //! Number of columns in the matrix read in from alist file
        unsigned int d_num_cols;

        //! GSL matrix structure for the parity check matrix
        gsl_matrix *d_H_ptr;

        /*!
         * \brief Read the matrix from a file in alist format
         * \param filename Name of an alist file to use. The alist
         *                 format is described at:
         *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
         */
        gsl_matrix *read_matrix_from_file(const std::string filename);

        //! Flag for whether or not the parity bits come first or last
        bool d_par_bits_last;

      public:
        //! Returns the parity check matrix H (needed by decoder)
        const gsl_matrix *H() const;

        //!Get the codeword length n
        unsigned int n() const;

        //! Get the information word length k
        unsigned int k() const;

        //! Subtract matrices using mod2 operations
        gsl_matrix *add_matrices_mod2(const gsl_matrix *,
                                      const gsl_matrix *) const;

        //! Multiply matrices using mod2 operations
        gsl_matrix *mult_matrices_mod2(const gsl_matrix *,
                                       const gsl_matrix *) const;

        //! Invert a square matrix using mod2 operations
        gsl_matrix *calc_inverse_mod2(const gsl_matrix *) const;

        /*!
         * \brief Get Boolean for whether or not parity bits come first or last
         * \details
         * The decoder will need to know if the parity bits are
         * coming first or last
         */
        bool parity_bits_come_last() const;

        virtual ~fec_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_fec_mtrx_H */
