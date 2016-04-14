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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fec_mtrx_impl.h"
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace gr {
  namespace fec {
    namespace code {

      // For use when casting to a matrix_sptr to provide the proper
      // callback to free the memory when the reference count goes to
      // 0. Needed to know how to cast from our matrix to gsl_matrix.
      void matrix_free(matrix *x)
      {
        gsl_matrix_free((gsl_matrix*)x);
      }


      matrix_sptr
      read_matrix_from_file(const std::string filename)
      {
        std::ifstream inputFile;
        unsigned int ncols, nrows;

        // Open the alist file (needs a C-string)
        inputFile.open(filename.c_str());
        if(!inputFile) {
          std::stringstream s;
          s << "There was a problem opening file '"
            << filename << "' for reading.";
          throw std::runtime_error(s.str());
        }

        // First line of file is matrix size: # columns, # rows
        inputFile >> ncols >> nrows;

        // Now we can allocate memory for the GSL matrix
        gsl_matrix *temp_matrix = gsl_matrix_alloc(nrows, ncols);
        gsl_matrix_set_zero(temp_matrix);

        // The next few lines in the file are not necessary in
        // constructing the matrix.
        std::string tempBuffer;
        unsigned int counter;
        for(counter = 0; counter < 4; counter++) {
          getline(inputFile, tempBuffer);
        }

        // These next lines list the indices for where the 1s are in
        // each column.
        unsigned int column_count = 0;
        std::string row_number;

        while(column_count < ncols) {
          getline(inputFile, tempBuffer);
          std::stringstream ss(tempBuffer);

          while(ss >> row_number) {
            int row_i = atoi(row_number.c_str());
            // alist files index starting from 1, not 0, so decrement
            row_i--;
            // set the corresponding matrix element to 1
            gsl_matrix_set(temp_matrix, row_i, column_count, 1);
          }
          column_count++;
        }

        // Close the alist file
        inputFile.close();

        // Stash the pointer
        matrix_sptr H = matrix_sptr((matrix*)temp_matrix, matrix_free);

        return H;
      }

      void
      write_matrix_to_file(const std::string filename, matrix_sptr M)
      {
        std::ofstream outputfile;

        // Open the output file
        outputfile.open(filename.c_str());
        if(!outputfile) {
          std::stringstream s;
          s << "There was a problem opening file '"
            << filename << "' for writing.";
          throw std::runtime_error(s.str());
        }

        unsigned int ncols = M->size2;
        unsigned int nrows = M->size1;
        std::vector<unsigned int> colweights(ncols, 0);
        std::vector<unsigned int> rowweights(nrows, 0);
        std::stringstream colout;
        std::stringstream rowout;

        for(unsigned int c = 0; c < ncols; c++) {
          for(unsigned int r = 0; r < nrows; r++) {
            double x = gsl_matrix_get((gsl_matrix*)(M.get()), r, c);
            if(x == 1) {
              colout << (r + 1) << " ";
              colweights[c]++;
            }
          }
          colout << std::endl;
        }

        for(unsigned int r = 0; r < nrows; r++) {
          for(unsigned int c = 0; c < ncols; c++) {
            double x = gsl_matrix_get((gsl_matrix*)(M.get()), r, c);
            if(x == 1) {
              rowout << (c + 1) << " ";
              rowweights[r]++;
            }
          }
          rowout << std::endl;
        }

        outputfile << ncols << " " << nrows << std::endl;
        outputfile << (*std::max_element(colweights.begin(), colweights.end())) << " "
                   << (*std::max_element(rowweights.begin(), rowweights.end())) << std::endl;

        std::vector<unsigned int>::iterator itr;
        for(itr = colweights.begin(); itr != colweights.end(); itr++) {
          outputfile << (*itr) << " ";
        }
        outputfile << std::endl;

        for(itr = rowweights.begin(); itr != rowweights.end(); itr++) {
          outputfile << (*itr) << " ";
        }
        outputfile << std::endl;

        outputfile << colout.str() << rowout.str();

        // Close the alist file
        outputfile.close();
      }

      matrix_sptr
      generate_G_transpose(matrix_sptr H_obj)
      {
        unsigned int k = H_obj->size1;
        unsigned int n = H_obj->size2;
        unsigned int row_index, col_index;

        gsl_matrix *G_transp = gsl_matrix_alloc(n, k);

        // Grab P' matrix (P' denotes P transposed)
        gsl_matrix *P_transpose = gsl_matrix_alloc(n-k, k);
        for(row_index = 0; row_index < n-k; row_index++) {
          for(col_index = 0; col_index < k; col_index++) {
            int value = gsl_matrix_get((gsl_matrix*)(H_obj.get()),
                                       row_index, col_index);
            gsl_matrix_set(P_transpose, row_index, col_index, value);
          }
        }

        // Set G transpose matrix (used for encoding)
        gsl_matrix_set_zero(G_transp);
        for(row_index = 0; row_index < k; row_index++) {
          col_index = row_index;
          gsl_matrix_set(G_transp, row_index, col_index, 1);
        }
        for(row_index = k; row_index < n; row_index++) {
          for(col_index = 0; col_index < k; col_index++) {
            int value = gsl_matrix_get(P_transpose, row_index - k, col_index);
            gsl_matrix_set(G_transp, row_index, col_index, value);
          }
        }

        // Stash the pointer
        matrix_sptr G = matrix_sptr((matrix*)G_transp, matrix_free);

        // Free memory
        gsl_matrix_free(P_transpose);

        return G;
      }

      matrix_sptr
      generate_G(matrix_sptr H_obj)
      {
        matrix_sptr G_trans = generate_G_transpose(H_obj);

        unsigned int k = H_obj->size1;
        unsigned int n = H_obj->size2;
        gsl_matrix *G = gsl_matrix_alloc(k, n);

        gsl_matrix_transpose_memcpy(G, (gsl_matrix*)(G_trans.get()));

        matrix_sptr Gret = matrix_sptr((matrix*)G, matrix_free);
        return Gret;
      }

      matrix_sptr
      generate_H(matrix_sptr G_obj)
      {
        unsigned int row_index, col_index;

        unsigned int n = G_obj->size2;
        unsigned int k = G_obj->size1;

        gsl_matrix *G_ptr = (gsl_matrix*)(G_obj.get());
        gsl_matrix *H_ptr = gsl_matrix_alloc(n-k, n);

        // Grab P matrix
        gsl_matrix *P = gsl_matrix_alloc(k, n-k);
        for(row_index = 0; row_index < k; row_index++) {
          for(col_index = 0; col_index < n-k; col_index++) {
            int value = gsl_matrix_get(G_ptr, row_index, col_index + k);
            gsl_matrix_set(P, row_index, col_index, value);
          }
        }

        // Calculate P transpose
        gsl_matrix *P_transpose = gsl_matrix_alloc(n-k, k);
        gsl_matrix_transpose_memcpy(P_transpose, P);

        // Set H matrix. H = [-P' I] but since we are doing mod 2,
        // -P = P, so H = [P' I]
        gsl_matrix_set_zero(H_ptr);
        for(row_index = 0; row_index < n-k; row_index++) {
          for(col_index = 0; col_index < k; col_index++) {
            int value = gsl_matrix_get(P_transpose, row_index, col_index);
            gsl_matrix_set(H_ptr, row_index, col_index, value);
          }
        }

        for(row_index = 0; row_index < n-k; row_index++) {
          col_index = row_index + k;
          gsl_matrix_set(H_ptr, row_index, col_index, 1);
        }

        // Free memory
        gsl_matrix_free(P);
        gsl_matrix_free(P_transpose);

        matrix_sptr H = matrix_sptr((matrix*)H_ptr, matrix_free);
        return H;
      }


      void
      print_matrix(const matrix_sptr M, bool numpy)
      {
        if(!numpy) {
          for(size_t i = 0; i < M->size1; i++) {
            for(size_t j = 0; j < M->size2; j++) {
              std::cout << gsl_matrix_get((gsl_matrix*)(M.get()), i, j) << " ";
            }
            std::cout << std::endl;
          }
          std::cout << std::endl;
        }
        else {
          std::cout << "numpy.matrix([ ";
          for(size_t i = 0; i < M->size1; i++) {
            std::cout << "[ ";
            for(size_t j = 0; j < M->size2; j++) {
              std::cout << gsl_matrix_get((gsl_matrix*)(M.get()), i, j) << ", ";
            }
            std::cout << "], ";
          }
          std::cout << "])" << std::endl;
        }
      }


      fec_mtrx_impl::fec_mtrx_impl()
      {
        // Assume the convention that parity bits come last in the
        // codeword
        d_par_bits_last = true;
      }

      const gsl_matrix*
      fec_mtrx_impl::H() const
      {
        const gsl_matrix *H_ptr = (gsl_matrix*)(d_H_sptr.get());
        return H_ptr;
      }

      unsigned int
      fec_mtrx_impl::n() const
      {
        return d_n;
      }

      unsigned int
      fec_mtrx_impl::k() const
      {
        return d_k;
      }

      void
      fec_mtrx_impl::add_matrices_mod2(gsl_matrix *result,
                                       const gsl_matrix *matrix1,
                                       const gsl_matrix *matrix2) const
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
      }

      void
      fec_mtrx_impl::mult_matrices_mod2(gsl_matrix *result,
                                        const gsl_matrix *matrix1,
                                        const gsl_matrix *matrix2) const
      {
        // Verify that matrix sizes are appropriate
        unsigned int a = (*matrix1).size1;  // # of rows
        unsigned int b = (*matrix1).size2;  // # of columns
        unsigned int c = (*matrix2).size1;  // # of rows
        unsigned int d = (*matrix2).size2;  // # of columns
        if (b != c) {
          std::cout << "Error in "
                    << "fec_mtrx_impl::mult_matrices_mod2."
                    << " Matrix dimensions do not allow for matrix "
                    <<   "multiplication operation:\nmatrix1 is "
                    << a << " x " << b << ", and matrix2 is " << c
                    << " x " << d << ".\n";
          exit(1);
        }

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
      }

      gsl_matrix*
      fec_mtrx_impl::calc_inverse_mod2(const gsl_matrix *original_matrix) const
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
        mult_matrices_mod2(test, original_matrix, matrix_inverse);

        gsl_matrix *identity = gsl_matrix_alloc(n,n);
        gsl_matrix_set_identity(identity);
        //int test_if_equal = gsl_matrix_equal(identity,test);
        gsl_matrix_sub(identity, test); // should be null set if equal

        double test_if_not_equal = gsl_matrix_max(identity);

        if(test_if_not_equal > 0) {
          throw "Error in calc_inverse_mod2(): The matrix inverse found is not valid.\n";
        }

        return matrix_inverse;
      }

      bool
      fec_mtrx_impl::parity_bits_come_last() const
      {
        return d_par_bits_last;
      }

      fec_mtrx_impl::~fec_mtrx_impl()
      {

      }

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
