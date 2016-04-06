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

#ifndef INCLUDED_ldpc_G_matrix_impl_H
#define INCLUDED_ldpc_G_matrix_impl_H

#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "fec_mtrx_impl.h"
#include <gnuradio/fec/ldpc_G_matrix.h>
#include <gnuradio/fec/ldpc_H_matrix.h>
#include <gnuradio/logger.h>

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
      class ldpc_G_matrix_impl
        : public fec_mtrx_impl, public ldpc_G_matrix
      {
      private:
        // GSL matrix structure for transpose of G
        gsl_matrix *d_G_transp_ptr;

        gsl_matrix *d_H_obj;

        //! Get the generator matrix (used during encoding)
        const gsl_matrix *G_transpose() const;

        gr::logger_ptr d_logger;
        gr::logger_ptr d_debug_logger;

      public:
        ldpc_G_matrix_impl(const std::string filename);

        void encode(unsigned char *outbuffer,
                    const unsigned char *inbuffer) const;

        void decode(unsigned char *outbuffer,
                    const float *inbuffer,
                    unsigned int frame_size,
                    unsigned int max_iterations) const;

        unsigned int n() const { return fec_mtrx_impl::n(); }

        unsigned int k() const { return fec_mtrx_impl::k(); }

        gsl_matrix* generate_H();

        gr::fec::code::fec_mtrx_sptr get_base_sptr();

        /*!
         * \brief Destructor
         * \details
         * Calls the gsl_matrix_free function to free memory.
         */
        virtual ~ldpc_G_matrix_impl();
      };

    }
  }
}

#endif /* INCLUDED_ldpc_G_matrix_impl_H */
