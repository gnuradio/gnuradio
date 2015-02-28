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

#ifndef INCLUDED_ldpc_gen_mtrx_H
#define INCLUDED_ldpc_gen_mtrx_H
 
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include <gnuradio/fec/fec_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      class FEC_API ldpc_gen_mtrx : public fec_mtrx
      {
      private:
        // GSL matrix structure for transpose of G
        gsl_matrix *d_G_transp_ptr;
        // Swig needs the scope resolution operator here
        gr::fec::code::fec_mtrx *d_base_ptr;
        
      public:
        ldpc_gen_mtrx(const std::string filename);
        // Default constructor, should not be used
        ldpc_gen_mtrx();
        // Get the generator matrix (used during encoding)
        const gsl_matrix *G_transpose() const;
        // A pointer to make swig work for the ldpc_bit_flip_decoder
        // GRC block
        gr::fec::code::fec_mtrx *get_base_ptr();

        // Destructor
        virtual ~ldpc_gen_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_ldpc_gen_mtrx_H */