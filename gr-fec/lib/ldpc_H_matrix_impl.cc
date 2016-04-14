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

#include "ldpc_H_matrix_impl.h"
#include <math.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace gr {
  namespace fec {
    namespace code {

      ldpc_H_matrix::sptr
      ldpc_H_matrix::make(const std::string filename, unsigned int gap)
      {
        return ldpc_H_matrix::sptr
          (new ldpc_H_matrix_impl(filename, gap));
      }

      ldpc_H_matrix_impl::ldpc_H_matrix_impl(const std::string filename, unsigned int gap)
        : fec_mtrx_impl()
      {
        matrix_sptr x = read_matrix_from_file(filename);
        d_num_cols = x->size2;
        d_num_rows = x->size1;
        d_gap = gap;

        // Make an actual copy so we guarantee that we're not sharing
        // memory with another class that reads the same alist file.
        gsl_matrix *temp_mtrx = gsl_matrix_alloc(d_num_rows, d_num_cols);
        gsl_matrix_memcpy(temp_mtrx, (gsl_matrix*)(x.get()));
        d_H_sptr = matrix_sptr((matrix*)temp_mtrx, matrix_free);

        // Length of codeword = # of columns
        d_n = d_num_cols;

        // Length of information word = (# of columns) - (# of rows)
        d_k = d_num_cols - d_num_rows;

        set_parameters_for_encoding();

        // The parity bits come first in this particular matrix
        // format (specifically required for the Richardson Urbanke
        // encoder)
        d_par_bits_last = false;

      } // Constructor

      const gsl_matrix*
      ldpc_H_matrix_impl::A() const
      {
        const gsl_matrix *A_ptr = &d_A_view.matrix;
        return A_ptr;
      }

      const gsl_matrix*
      ldpc_H_matrix_impl::B() const
      {
        const gsl_matrix *B_ptr = &d_B_view.matrix;
        return B_ptr;
      }

      const gsl_matrix*
      ldpc_H_matrix_impl::D() const
      {
        const gsl_matrix *D_ptr = &d_D_view.matrix;
        return D_ptr;
      }

      const gsl_matrix*
      ldpc_H_matrix_impl::E() const
      {
        const gsl_matrix *E_ptr = &d_E_view.matrix;
        return E_ptr;
      }

      const gsl_matrix*
      ldpc_H_matrix_impl::T() const
      {
        const gsl_matrix *T_ptr = &d_T_view.matrix;
        return T_ptr;
      }

      const gsl_matrix*
      ldpc_H_matrix_impl::phi_inverse() const
      {
        const gsl_matrix *phi_inverse_ptr = d_phi_inverse_ptr;
        return phi_inverse_ptr;
      }

      void
      ldpc_H_matrix_impl::set_parameters_for_encoding()
      {

        // This function defines all of the submatrices that will be
        // needed during encoding.

        unsigned int t = d_num_rows - d_gap;

        // T submatrix
        d_T_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                        0, 0, t, t);

        gsl_matrix *d_T_inverse_ptr;
        try {
          d_T_inverse_ptr = calc_inverse_mod2(&d_T_view.matrix);
        }
        catch (char const *exceptionString) {
          std::cout << "Error in set_parameters_for_encoding while "
                    << "looking for inverse T matrix: "
                    << exceptionString
                    << "Tip: verify that the correct gap is being "
                    << "specified for this alist file.\n";

          throw std::runtime_error("set_parameters_for_encoding");
        }

        // E submatrix
        d_E_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                        t, 0, d_gap, d_n-d_k-d_gap);

        // A submatrix
        d_A_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                        0, t, t, d_gap);

        // C submatrix (used to find phi but not during encoding)
        gsl_matrix_view C_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                                      t, t, d_gap, d_gap);

        // These are just temporary matrices used to find phi.
        gsl_matrix *temp1 = gsl_matrix_alloc(d_E_view.matrix.size1, d_T_inverse_ptr->size2);
        mult_matrices_mod2(temp1, &d_E_view.matrix, d_T_inverse_ptr);

        gsl_matrix *temp2 = gsl_matrix_alloc(temp1->size1, d_A_view.matrix.size2);
        mult_matrices_mod2(temp2, temp1, &d_A_view.matrix);

        // Solve for phi.
        gsl_matrix *phi = gsl_matrix_alloc(C_view.matrix.size1, temp2->size2);
        add_matrices_mod2(phi, &C_view.matrix, temp2);

        // If phi has at least one nonzero entry, try for inverse.
        if (gsl_matrix_max(phi)) {
          try {
            gsl_matrix *inverse_phi = calc_inverse_mod2(phi);

            // At this point, an inverse was found.
            d_phi_inverse_ptr = inverse_phi;

          }
          catch (char const *exceptionString) {

            std::cout << "Error in set_parameters_for_encoding while"
                      << " finding inverse_phi: " << exceptionString
                      << "Tip: verify that the correct gap is being "
                      << "specified for this alist file.\n";
            throw std::runtime_error("set_parameters_for_encoding");
          }
        }

        // B submatrix
        d_B_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                        0, t + d_gap, t, d_n-d_gap-t);

        // D submatrix
        d_D_view = gsl_matrix_submatrix((gsl_matrix*)(d_H_sptr.get()),
                                        t, t + d_gap, d_gap, d_n-d_gap-t);

        // Free memory
        gsl_matrix_free(temp1);
        gsl_matrix_free(temp2);
        gsl_matrix_free(phi);
        gsl_matrix_free(d_T_inverse_ptr);
      }

      void
      ldpc_H_matrix_impl::back_solve_mod2(gsl_matrix *x,
                                          const gsl_matrix *U,
                                          const gsl_matrix *y) const
      {
        // Exploit the fact that the matrix T is upper triangular and
        // sparse. In the steps to find p1 and p2, back solve rather
        // than do matrix multiplication to reduce number of
        // operations required.

        // Form is Ux = y where U is upper triangular and y is column
        // vector. Solve for x.

        // Allocate memory for the result
        int num_rows   = (*U).size1;
        int num_cols_U = (*U).size2;

        // Back solve
        for (int i = num_rows-1; i >= 0; i--) {
          // x[i] = y[i]
          gsl_matrix_set(x, i, 0, gsl_matrix_get(y, i, 0));

          int j;
          for (j = i+1; j < num_cols_U; j++) {
            int U_i_j = gsl_matrix_get(U, i, j);
            int x_i   = gsl_matrix_get(x, i, 0);
            int x_j   = gsl_matrix_get(x, j, 0);
            int temp1 = (U_i_j * x_j) % 2;
            int temp2 = (x_i + temp1) % 2;
            gsl_matrix_set(x, i, 0, temp2);
          }
          // Perform x[i] /= U[i,i], GF(2) operations
          int U_i_i = gsl_matrix_get(U, i, i);
          int x_i   = gsl_matrix_get(x, i, 0);
          if(x_i==0 && U_i_i==1)
            gsl_matrix_set(x, i, 0, 0);
          else if (x_i==0 && U_i_i==0)
            gsl_matrix_set(x, i, 0, 0);
          else if (x_i==1 && U_i_i==1)
            gsl_matrix_set(x, i, 0, 1);
          else if (x_i==1 && U_i_i==0)
            std::cout << "Error in "
                      << " ldpc_H_matrix_impl::back_solve_mod2,"
                      << " division not defined.\n";
          else
            std::cout << "Error in ldpc_H_matrix::back_solve_mod2\n";
        }
      }


      void
      ldpc_H_matrix_impl::encode(unsigned char *outbuffer,
                                 const unsigned char *inbuffer) const
      {

        // Temporary matrix for storing stages of encoding.
        gsl_matrix *s, *p1, *p2;
        gsl_matrix *temp1, *temp2, *temp3, *temp4, *temp5, *temp6, *temp7;

        unsigned int index, k = d_k;
        s = gsl_matrix_alloc(k, 1);
        for (index = 0; index < k; index++) {
          double value = static_cast<double>(inbuffer[index]);
          gsl_matrix_set(s, index, 0, value);
        }

        // Solve for p2 (parity part). By using back substitution,
        // the overall complexity of determining p2 is O(n + g^2).
        temp1 = gsl_matrix_alloc(B()->size1, s->size2);
        mult_matrices_mod2(temp1, B(), s);

        temp2 = gsl_matrix_alloc(T()->size1, 1);
        back_solve_mod2(temp2, T(), temp1);

        temp3 = gsl_matrix_alloc(E()->size1, temp2->size2);
        mult_matrices_mod2(temp3, E(), temp2);

        temp4 = gsl_matrix_alloc(D()->size1, s->size2);
        mult_matrices_mod2(temp4, D(), s);

        temp5 = gsl_matrix_alloc(temp4->size1, temp3->size2);
        add_matrices_mod2(temp5, temp4, temp3);

        p2 = gsl_matrix_alloc(phi_inverse()->size1, temp5->size2);
        mult_matrices_mod2(p2, phi_inverse(), temp5);

        // Solve for p1 (parity part). By using back substitution,
        // the overall complexity of determining p1 is O(n).
        temp6 = gsl_matrix_alloc(A()->size1, p2->size2);
        mult_matrices_mod2(temp6, A(), p2);

        temp7 = gsl_matrix_alloc(temp6->size1, temp1->size2);
        add_matrices_mod2(temp7, temp6, temp1);

        p1 = gsl_matrix_alloc(T()->size1, 1);
        back_solve_mod2(p1, T(), temp7);

        // Populate the codeword to be output
        unsigned int p1_length = (*p1).size1;
        unsigned int p2_length = (*p2).size1;
        for (index = 0; index < p1_length; index++) {
          int value = gsl_matrix_get(p1, index, 0);
          outbuffer[index] = value;
        }
        for (index = 0; index < p2_length; index++) {
          int value = gsl_matrix_get(p2, index, 0);
          outbuffer[p1_length+index] = value;
        }
        for (index = 0; index < k; index++) {
          int value = gsl_matrix_get(s, index, 0);
          outbuffer[p1_length+p2_length+index] = value;
        }

        // Free temporary matrices
        gsl_matrix_free(temp1);
        gsl_matrix_free(temp2);
        gsl_matrix_free(temp3);
        gsl_matrix_free(temp4);
        gsl_matrix_free(temp5);
        gsl_matrix_free(temp6);
        gsl_matrix_free(temp7);
        gsl_matrix_free(p1);
        gsl_matrix_free(p2);
      }


      void
      ldpc_H_matrix_impl::decode(unsigned char *outbuffer,
                                 const float *inbuffer,
                                 unsigned int frame_size,
                                 unsigned int max_iterations) const
      {
        unsigned int index, n = d_n;
        gsl_matrix *x = gsl_matrix_alloc(n, 1);
        for (index = 0; index < n; index++) {
          double value = inbuffer[index] > 0 ? 1.0 : 0.0;
          gsl_matrix_set(x, index, 0, value);
        }

        // Initialize counter
        unsigned int count = 0;

        // Calculate syndrome
        gsl_matrix *syndrome = gsl_matrix_alloc(H()->size1, x->size2);
        mult_matrices_mod2(syndrome, H(), x);

        // Flag for finding a valid codeword
        bool found_word = false;

        // If the syndrome is all 0s, then codeword is valid and we
        // don't need to loop; we're done.
        if (gsl_matrix_isnull(syndrome)) {
          found_word = true;
        }

        // Loop until valid codeword is found, or max number of
        // iterations is reached, whichever comes first
        while ((count < max_iterations) && !found_word) {
          // For each of the n bits in the codeword, determine how
          // many of the unsatisfied parity checks involve that bit.
          // To do this, first find the nonzero entries in the
          // syndrome. The entry numbers correspond to the rows of
          // interest in H.
          std::vector<int> rows_of_interest_in_H;
          for (index = 0; index < (*syndrome).size1; index++) {
            if (gsl_matrix_get(syndrome, index, 0)) {
              rows_of_interest_in_H.push_back(index);
            }
          }

          // Second, for each bit, determine how many of the
          // unsatisfied parity checks involve this bit and store
          // the count.
          unsigned int i, col_num, n = d_n;
          std::vector<int> counts(n,0);
          for (i = 0; i < rows_of_interest_in_H.size(); i++) {
            unsigned int row_num = rows_of_interest_in_H[i];
            for (col_num = 0; col_num < n; col_num++) {
              double value = gsl_matrix_get(H(),
                                            row_num,
                                            col_num);
              if (value > 0) {
                counts[col_num] = counts[col_num] + 1;
              }
            }
          }

          // Next, determine which bit(s) is associated with the most
          // unsatisfied parity checks, and flip it/them.
          int max = 0;
          for (index = 0; index < n; index++) {
            if (counts[index] > max) {
              max = counts[index];
            }
          }

          for (index = 0; index < n; index++) {
            if (counts[index] == max) {
              unsigned int value = gsl_matrix_get(x, index, 0);
              unsigned int new_value = value ^ 1;
              gsl_matrix_set(x, index, 0, new_value);
            }
          }

          // Check the syndrome; see if valid codeword has been found
          mult_matrices_mod2(syndrome, H(), x);
          if (gsl_matrix_isnull(syndrome)) {
            found_word = true;
            break;
          }
          count++;
        }

        // Extract the info word and assign to output. This will
        // happen regardless of if a valid codeword was found.
        if(parity_bits_come_last()) {
          for(index = 0; index < frame_size; index++) {
            outbuffer[index] = gsl_matrix_get(x, index, 0);
          }
        }
        else {
          for(index = 0; index < frame_size; index++) {
            unsigned int i = index + n - frame_size;
            int value = gsl_matrix_get(x, i, 0);
            outbuffer[index] = value;
          }
        }

        // Free memory
        gsl_matrix_free(syndrome);
        gsl_matrix_free(x);
      }

      gr::fec::code::fec_mtrx_sptr
      ldpc_H_matrix_impl::get_base_sptr()
      {
        return shared_from_this();
      }

      ldpc_H_matrix_impl::~ldpc_H_matrix_impl()
      {

        // Call the gsl_matrix_free function to free memory.
        gsl_matrix_free (d_phi_inverse_ptr);
      }
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */
