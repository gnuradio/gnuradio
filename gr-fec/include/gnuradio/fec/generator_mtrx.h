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

#ifndef INCLUDED_generator_mtrx_H
#define INCLUDED_generator_mtrx_H
 
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include <gnuradio/fec/fec_mtrx.h>

namespace gr {
  namespace fec {
    namespace code {
      class FEC_API generator_mtrx : public fec_mtrx
      {
      private:
        // GSL matrix structure for the generator matrix (encode)
        gsl_matrix *d_G_ptr;
        
      public:
        generator_mtrx(const std::string filename);
        // Default constructor, should not be used
        generator_mtrx();
        // Get the generator matrix (used during encoding)
        const gsl_matrix *G();

        // Destructor
        virtual ~generator_mtrx();
      };
    }
  }
}

#endif /* INCLUDED_generator_mtrx_H */