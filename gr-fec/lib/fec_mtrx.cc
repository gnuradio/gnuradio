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

#include <gnuradio/fec/fec_mtrx.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      fec_mtrx::fec_mtrx()
      {
        // Assume the convention that parity bits come last in the
        // codeword
        d_par_bits_last = true;
      }

      gsl_matrix*
      fec_mtrx::read_matrix_from_file(const std::string filename)
      {
        /* This function reads in an alist file and creates the
           corresponding matrix. The format of alist files is 
           described at:
           http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
        */
        std::ifstream inputFile;

        // Open the alist file (needs a C-string)
        inputFile.open((filename).c_str());
        if (!inputFile) {
          std::cout << "There was a problem opening file "
                    << filename << " for reading.\n";
          exit(1);
        }

        // First line of file is matrix size: # columns, # rows
        inputFile >> d_num_cols >> d_num_rows; 

        // Now we can allocate memory for the GSL matrix
        gsl_matrix *temp_matrix = gsl_matrix_alloc(d_num_rows,
                                                   d_num_cols);
        gsl_matrix_set_zero(temp_matrix);

        // The next few lines in the file are not necessary in
        // constructing the matrix.
        std::string tempBuffer;
        unsigned int counter;
        for (counter = 0; counter < 4; counter++) {
          getline(inputFile,tempBuffer);
        }

        // These next lines list the indices for where the 1s are in
        // each column.
        unsigned int column_count = 0;
        std::string row_number;

        while (column_count < d_num_cols) {
          getline(inputFile,tempBuffer);
          std::stringstream ss(tempBuffer);

          while (ss >> row_number) {
            int row_i = atoi(row_number.c_str());
            // alist files index starting from 1, not 0, so decrement
            row_i--;
            // set the corresponding matrix element to 1
            gsl_matrix_set(temp_matrix,row_i,column_count,1);
          }
          column_count++;
        }

        // The subsequent lines in the file list the indices for
        // where the 1s are in the rows, but this is redundant
        // information that we already have. 

        // Close the alist file
        inputFile.close();

        return temp_matrix;
      }

      const gsl_matrix*
      fec_mtrx::H() const
      {
        const gsl_matrix *H_ptr = d_H_ptr;
        return H_ptr; 
      }

      unsigned int
      fec_mtrx::n() const
      {
        return d_n;
      }

      unsigned int
      fec_mtrx::k() const
      {
        return d_k;
      }

      gsl_matrix*
      fec_mtrx::add_matrices_mod2(const gsl_matrix *matrix1, const gsl_matrix *matrix2) const
      {
        // This function returns ((matrix1 + matrix2) % 2). 

        // Verify that matrix sizes are appropriate
        unsigned int matrix1_rows = (*matrix1).size1;
        unsigned int matrix1_cols = (*matrix1).size2;
        unsigned int matrix2_rows = (*matrix2).size1;
        unsigned int matrix2_cols = (*matrix2).size2;

        if (matrix1_rows != matrix2_rows) {
          std::cout << "Error in add_matrices_mod2. Matrices do"
                    << " not have the same number of rows.\n";
          exit(1);
        }
        if (matrix1_cols != matrix2_cols) {
          std::cout << "Error in add_matrices_mod2. Matrices do"
                    << " not have the same number of columns.\n";
          exit(1);
        }

        // Allocate memory for the result
        gsl_matrix *result = gsl_matrix_alloc(matrix1_rows,
                                              matrix1_cols);

        // Copy matrix1 into result
        gsl_matrix_memcpy(result, matrix1);

        // Do subtraction. This is not mod 2 yet.
        gsl_matrix_add(result, matrix2);

        // Take care of mod 2 manually
        unsigned int row_index, col_index;
        for (row_index = 0; row_index < matrix1_rows; row_index++) {
          for (col_index = 0; col_index < matrix1_cols;col_index++) {
            int value = gsl_matrix_get(result, row_index, col_index);
            int new_value = abs(value) % 2;
            gsl_matrix_set(result, row_index, col_index, new_value);
          }
        }

        return result; 
      }

      gsl_matrix*
      fec_mtrx::mult_matrices_mod2(const gsl_matrix *matrix1, const gsl_matrix *matrix2) const
      {
        // Verify that matrix sizes are appropriate
        unsigned int a = (*matrix1).size1;  // # of rows
        unsigned int b = (*matrix1).size2;  // # of columns
        unsigned int c = (*matrix2).size1;  // # of rows
        unsigned int d = (*matrix2).size2;  // # of columns
        if (b != c) {
          std::cout << "Error in "
                    << "fec_mtrx::mult_matrices_mod2."
                    << " Matrix dimensions do not allow for matrix "
                    <<   "multiplication operation:\nmatrix1 is " 
                    << a << " x " << b << ", and matrix2 is " << c
                    << " x " << d << ".\n";
          exit(1);
        }

        // Allocate memory for the result.
        gsl_matrix *result = gsl_matrix_alloc(a,d);

        // Perform matrix multiplication. This is not mod 2.
        gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, matrix1,
                        matrix2, 0.0, result);

        // Take care of mod 2 manually.
        unsigned int row_index, col_index;
        unsigned int rows = (*result).size1, 
        cols = (*result).size2;
        for (row_index = 0; row_index < rows; row_index++) {
          for (col_index = 0; col_index < cols; col_index++) {
            int value = gsl_matrix_get(result, row_index,col_index);
            int new_value = value % 2;
            gsl_matrix_set(result, row_index, col_index, new_value);
          }
        }
        return result;
      }

      gsl_matrix*
      fec_mtrx::calc_inverse_mod2(const gsl_matrix *original_matrix) const
      {

        // Let n represent the size of the n x n square matrix
        unsigned int n = (*original_matrix).size1;
        unsigned int row_index, col_index;

        // Make a copy of the original matrix, call it matrix_altered.
        // This matrix will be modified by the GSL functions. 
        gsl_matrix *matrix_altered = gsl_matrix_alloc(n, n);
        gsl_matrix_memcpy(matrix_altered, original_matrix);

        // In order to find the inverse, GSL must perform a LU 
        // decomposition first. 
        gsl_permutation *permutation = gsl_permutation_alloc(n);
        int signum;
        gsl_linalg_LU_decomp(matrix_altered, permutation, &signum);

        // Allocate memory to store the matrix inverse
        gsl_matrix *matrix_inverse = gsl_matrix_alloc(n,n);

        // Find matrix inverse. This is not mod2.
        int status = gsl_linalg_LU_invert(matrix_altered,
                                          permutation,
                                          matrix_inverse);

        if (status) {
          // Inverse not found by GSL functions.
          throw "Error in calc_inverse_mod2(): inverse not found.\n";
        }

        // Find determinant
        float determinant = gsl_linalg_LU_det(matrix_altered,signum);

        // Multiply the matrix inverse by the determinant.
        gsl_matrix_scale(matrix_inverse, determinant);

        // Take mod 2 of each element in the matrix.
        for (row_index = 0; row_index < n; row_index++) {
          for (col_index = 0; col_index < n; col_index++) {

            float value = gsl_matrix_get(matrix_inverse,
                                         row_index,
                                         col_index);

            // take care of mod 2
            int value_cast_as_int = static_cast<int>(value);
            int temp_value = abs(fmod(value_cast_as_int,2));

            gsl_matrix_set(matrix_inverse,
                           row_index,
                           col_index,
                           temp_value);
          }
        } 

        int max_value = gsl_matrix_max(matrix_inverse);
        if (!max_value) {
          throw "Error in calc_inverse_mod2(): The matrix inverse found is all zeros.\n";
        }

        // Verify that the inverse was found by taking matrix
        // product of original_matrix and the inverse, which should
        // equal the identity matrix.
        gsl_matrix *test = gsl_matrix_alloc(n,n);
        gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0,
                        original_matrix, matrix_inverse, 0.0, test);
        
        // Have to take care of % 2 manually
         for (row_index = 0; row_index < n; row_index++) {
          for (col_index = 0; col_index < n; col_index++) {
            int value = gsl_matrix_get(test, row_index, col_index);
            int temp_value = value % 2;
            gsl_matrix_set(test, row_index, col_index, temp_value);
          }
        }  

        gsl_matrix *identity = gsl_matrix_alloc(n,n);
        gsl_matrix_set_identity(identity);
        int test_if_equal = gsl_matrix_equal(identity,test);

        if (!test_if_equal) {
          throw "Error in calc_inverse_mod2(): The matrix inverse found is not valid.\n";
        }

        return matrix_inverse;
      }
      bool
      fec_mtrx::parity_bits_come_last() const
      {
        return d_par_bits_last;
      }

      fec_mtrx::~fec_mtrx()
      {

      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */