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

#include <gnuradio/fec/generator_mtrx.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      generator_mtrx::generator_mtrx(const std::string filename) 
      {
        // Read the matrix from a file in alist format
        read_matrix_from_file(filename);

      } // Constructor

      // Default constructor, should not be used
      generator_mtrx::generator_mtrx()
      {
        std::cout << "Error in generator_mtrx(): Default "
                  << "constructor called.\nMust provide filename for"
                  << " generator matrix. \n\n";
        exit(1);
      } // Default constructor

      unsigned int
      generator_mtrx::n()
      {
        return d_n;
      }

      unsigned int
      generator_mtrx::k()
      {
        return d_k;
      }
      
      const gsl_matrix*
      generator_mtrx::G()
      {
        const gsl_matrix *G_ptr = d_G_ptr;
        return G_ptr; 
      }

      const gsl_matrix*
      generator_mtrx::H()
      {
        const gsl_matrix *H_ptr = d_H_ptr;
        return H_ptr; 
      }

      generator_mtrx::~generator_mtrx()
      {
        ///////////////////////////////////
        // TODO Call the gsl_matrix_free function to free memory.
        ///////////////////////////////////

      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */