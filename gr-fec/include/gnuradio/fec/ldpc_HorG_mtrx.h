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

#ifndef INCLUDED_ldpc_HorG_mtrx_H
#define INCLUDED_ldpc_HorG_mtrx_H
 
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include <gnuradio/fec/fec_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      /*!
       * \brief Class for storing H or G matrix
       * \ingroup error_coding_blk
       *
       * \details
       * This class stores a GSL matrix variable, specifically
       * either a:
       *
       * 1) Generator matrix, G, in the standard format G = [I P],
       *    where I is an identity matrix and P is the parity
       *    submatrix.
       *
       * or
       *
       * 2) Parity matrix, H, in the standard format H = [P' I],
       *    where P' is the transpose of the parity submatrix and I
       *    is an identity matrix.
       *
       * This variable can used by the ldpc_gen_mtrx_encoder and
       * ldpc_bit_flip_decoder classes.
       */
      class FEC_API ldpc_HorG_mtrx : public fec_mtrx
      {
      private:
        // GSL matrix structure for transpose of G
        gsl_matrix *d_G_transp_ptr;

        // SWIG needs the scope resolution operator here
        gr::fec::code::fec_mtrx *d_base_ptr;
        
      public:
        /*!
         * \brief Constructor given alist file
         * \details
         * 1. Reads in the matrix from an alist file
         * 2. Determines if the matrix format is G=[I P] or H=[P' I]
         * 3. Solves for G transpose (will be used during encoding)
         *
         * \param filename Name of an alist file to use. The alist
         *                 format is described at:
         *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
         */
        ldpc_HorG_mtrx(const std::string filename);

        //! Default constructor; should not be used
        ldpc_HorG_mtrx();

        //! Get the generator matrix (used during encoding)
        const gsl_matrix *G_transpose() const;

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
         * Calls the gsl_matrix_free function to free memory.
         */
        virtual ~ldpc_HorG_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_ldpc_HorG_mtrx_H */