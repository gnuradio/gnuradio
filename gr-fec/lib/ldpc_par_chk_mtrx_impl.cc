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

#include <LDPC_par_chk_mtrx_impl.h> 
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

namespace gr {
  namespace fec {
    namespace code {

      // Constructor if filename is given, case 1
      LDPC_par_chk_mtrx_impl::LDPC_par_chk_mtrx_impl(string *file, int flag, unsigned int gap)
      {
        if (flag == 1 && gap == 0) {
          cout << "Error in LDPC_par_chk_mtrx_impl constructor. If"
             << " encoding-ready alist file is provided, gap must be"
             << " specified.\n\n";
          exit(1);
        }

        // Read in matrix from file
        read_matrix_from_file(file);
        
        if (flag == 1 && gap > 0) {
          // If flag == 1 and g is nonzero: case 1a
          d_gap = gap;
          set_parameters_for_encoding();
        }
        else if (flag == 2) {
          // If flag == 2, case 1b
          try {
            full_rank_to_TABECD_form();
          }
          catch (char const *exceptionString) {
            cout << exceptionString;
          }

          preprocess_for_encoding();
          set_parameters_for_encoding();
        }
        else if (flag == 3) {
          // If flag == 3, case 1c
          reduce_H_to_full_rank_matrix();

          try {
            full_rank_to_TABECD_form();
          }
          catch (char const *exceptionString) {
            cout << exceptionString;
          }

          preprocess_for_encoding();
          set_parameters_for_encoding();
        }
        else if (flag == 4) {
          // If flag == 4, case 1d. Try to run as flag 3 case
          reduce_H_to_full_rank_matrix();

          try {
            full_rank_to_TABECD_form();
          }
          catch (char const *exceptionString) {
            cout << "Error in LDPC_par_chk_mtrx_impl() "
                 << "constructor: "
                 << exceptionString;
          }

          preprocess_for_encoding();
          set_parameters_for_encoding();
          // TODO add some try/catch blocks
        }
        else {
          cout << "Error in LDPC_par_chk_mtrx_impl constructor. "
               << "Invalid flag set in constructor arguments.\n\n";
          exit(1);
        }

        // Turn off GSL error handler so that program does not abort
        // if an error pops up.
        gsl_set_error_handler_off();

      } // Constructor if filename is given, case 1

      // Constructor if a parity check matrix is given, case 2
      LDPC_par_chk_mtrx_impl::LDPC_par_chk_mtrx_impl(gsl_matrix *m_ptr)
      {
        // Turn off GSL error handler so that program does not abort
        // if an error pops up.
        gsl_set_error_handler_off();

        d_H_ptr = m_ptr;

        // set n, the number of columns
        d_n = (*d_H_ptr).size2;

        // set the number of rows
        d_num_rows = (*d_H_ptr).size1;

        // Length of information word = num of columns-number of rows
        d_k = d_n - d_num_rows;

        // FIXME need to find the gap or have the user specify it

      } // Constructor if a parity check matrix is given, case 2

      // Constructor if n,p,q values are given, case 3
      LDPC_par_chk_mtrx_impl::LDPC_par_chk_mtrx_impl(unsigned int n, unsigned int p, unsigned int q)
      {
        /* This constructor accepts parameters to construct a
           regular low density parity check (LDPC) parity check
           matrix H. The algorithm follows Gallager's approach where
           we create p submatrices and stack them together. 
           Reference: Turbo Coding for Satellite and Wireless
           Communications, section 9,3.

           n = codeword length, or number of columns in the matrix
           p = column weight
           q = row weight

           Note: the matrices computed from this algorithm will never
           have full rank. (Reference Gallager's Dissertation.) They 
           will have rank = (number of rows - p + 1). To convert it
           to full rank, the function reduce_H_to_full_rank_matrix is
           called. Then, the function full_rank_to_TABECD_form is called
           to complete the preprocessing steps.
        */

        // TODO: there should probably be some guidelines for the n, 
        // p, and q values chosen, but I have not seen any specific
        // guidelines in the literature.

        // For this algorithm, n/q must be an integer, because the
        // ratio defines the number of rows in each submatrix.

        // Turn off GSL error handler so that program does not abort 
        // if an error pops up.
        gsl_set_error_handler_off();

        d_n = n;

        float test = d_n % q;
        if (test) {
          cout << "Error in regular LDPC code contructor algorithm: "
               <<"The ratio of inputs n/q must be a whole number.\n";
          exit(1);
        }

        // First, allocate memory for the H matrix (the big final matrix)
        d_num_rows = (d_n*p)/q;   // # of rows in the H matrix
        d_H_ptr = gsl_matrix_alloc(d_num_rows,d_n);
        gsl_matrix_set_zero(d_H_ptr);

        // Number of rows in each submatrix
        unsigned int sub_row_num = d_num_rows/p;

        // Define the first submatrix
        gsl_matrix *submatrix1_ptr = gsl_matrix_alloc(sub_row_num,
                                                      d_n);
        unsigned int row_number, range1, range2, index;
        for (row_number = 0; row_number < sub_row_num;row_number++) {
          range1 = row_number*q;
          range2 = (row_number+1)*q;
          index  = range1;

          while (index < range2) {
            gsl_matrix_set(submatrix1_ptr,row_number,index,1);
            index++;
          }
        }

        // Copy the first submatrix to the top of the H matrix
        unsigned int col_index, row_index, value; 
        for (col_index = 0; col_index < d_n; col_index++) {
          for (row_index=0; row_index < sub_row_num; row_index++) {
            value=gsl_matrix_get(submatrix1_ptr,row_index,col_index);
            gsl_matrix_set(d_H_ptr,row_index,col_index,value);
          }
        }

        // Create an array of the column numbers
        int column_numbers[d_n];
        for (index = 0; index < d_n; index ++) {
          column_numbers[index] = index;
        }

        // Setup the random number generator
        const gsl_rng_type *T;
        gsl_rng *rng;
        gsl_rng_env_setup();
        T = gsl_rng_default;
        rng = gsl_rng_alloc(T);

        // Create other submatrices and vertically stack them on 
        // below
        unsigned int submatrix_count = 2;
        while (submatrix_count <= p) {

          // Shuffle the array of column numbers. Call the function
          // twice. For some reason, the first and last entries
          // don't ever get shuffled if this function is only called
          // once. 
          gsl_ran_shuffle(rng,column_numbers,d_n,sizeof(int));
          gsl_ran_shuffle(rng,column_numbers,d_n,sizeof(int));

          // Use shuffled column order to build this submatrix
          for (col_index = 0; col_index < d_n; col_index++) {
            int reference_col_index = column_numbers[col_index];
            for (row_index=0; row_index < sub_row_num; row_index++) {
              value = gsl_matrix_get(submatrix1_ptr,
                                     row_index,
                                     reference_col_index);

              unsigned int row_index_in_H = row_index + 
                     (submatrix_count-1)*sub_row_num;

              gsl_matrix_set(d_H_ptr,
                             row_index_in_H,
                             col_index,
                             value);
            }
          }
          submatrix_count++;
        }

        reduce_H_to_full_rank_matrix();

        bool found_acceptable_H = false;

        while (!found_acceptable_H) {

          // full_rank_to_TABECD_form
          try {
            full_rank_to_TABECD_form();
          }
          catch (char const *exceptionString) {
            cout << "Error in LDPC_par_chk_mtrx_impl()"
                 << " constructor:\n" << exceptionString;
          }

          // preprocess_for_encoding
          try {
            preprocess_for_encoding();
          }
          catch (char const *exceptionString) {
            cout << "Error in preprocess_for_encoding() function:\n"
                 << exceptionString;
            continue;
          }

          // set_parameters_for_encoding
          try {
            set_parameters_for_encoding();

            // If we've made it this far, then we've found an
            // acceptable H matrix.
            found_acceptable_H = true;  
          }
          catch (char const *exceptionString) {
            cout << "Error in preprocess_for_encoding() function:\n"
                 << exceptionString;
          }
        }
      } // Constructor if n,p,q values are given, case 3

      // Default constructor, should not be used
      LDPC_par_chk_mtrx_impl::LDPC_par_chk_mtrx_impl() {
        cout << "Error in LDPC_par_chk_mtrx_impl(): Default "
             << "constructor called.\nMust provide arguments for one"
             << " of the constructors so that a parity check matrix "
             << "can be created. (Constructor for this class is "
             << "overloaded.)\n\n";
        exit(1);
      }

      LDPC_par_chk_mtrx_impl::~LDPC_par_chk_mtrx_impl()
      {
        // Call the gsl_matrix_free function to free the allocated
        // parity check matrix.
        gsl_matrix_free (d_H_ptr);
      }

      unsigned int LDPC_par_chk_mtrx_impl::get_n() {
        return d_n;
      }

      unsigned int LDPC_par_chk_mtrx_impl::get_k() {
        return d_k;
      }

      unsigned int LDPC_par_chk_mtrx_impl::get_rank() {
        // Returns the value set in reduce_H_to_full_rank_matrix().
        // If that function wasn't ran, then this variable hasn't
        // been set.

        // I was hoping to use a GSL function to calculate rank but
        // there doesn't seem to be a simple one for this.
        return d_rank;
      }

      void LDPC_par_chk_mtrx_impl::write_matrix_to_file(string *file) {
        /*
          This function writes an alist file for the parity check
          matrix. The format of alist files is desribed at: 
          http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
        */

        string alist_filename(*file);
        ofstream output_file;

        // Open the file (needs a C-string)
        output_file.open(alist_filename.c_str());
        if (!output_file) {
          cout << "Error: Could not open file " << alist_filename
               << " for writing.\n";
          exit(1);
        }

        // 1st line is number of columns and number of rows
        output_file << d_n << " " << d_num_rows << endl;

        // Initialize some variables
        string temp_string_1("");
        string temp_string_2("");
        string temp_string_3("");
        string temp_string_4("");
        unsigned int max_row_weight = 0, max_col_weight = 0;
        unsigned int row_index, col_index, row_weight, col_weight, value;

        // Use stringstream to convert unsigned int to string.
        stringstream out;

        for (row_index = 0; row_index < d_num_rows; row_index++) {
          row_weight = 0;
          for (col_index = 0; col_index < d_n; col_index++) {

            // Get the value from the H matrix.
            value = gsl_matrix_get(d_H_ptr, row_index,col_index);

            if (value) {
              // Increment the weight for this row.
              row_weight++;
              // Index from 1, not 0, in the alist format.
              out << col_index + 1;
              // Append this index to the string.
              temp_string_2 += out.str();
              // Now clear the stringstream
              out.str("");
              // Add a space too.
              temp_string_2 += " ";
            }
          }

          // Add a newline between rows
          temp_string_2 += "\n";

          // Check if the current row has the max weight.
          if (row_weight > max_row_weight) {
            max_row_weight = row_weight;
          }
          
          // Use stringstream to convert unsigned int to string
          out << row_weight;
          temp_string_1 += out.str(); // Append the row weight
          out.str("");                // Clear the stringstream
          temp_string_1 += " ";       // Add a space
        }

        for (col_index = 0; col_index < d_n; col_index++) {
          col_weight = 0;
          for (row_index = 0; row_index < d_num_rows; row_index++) {
            
            // Get the value from the H matrix.
            value = gsl_matrix_get(d_H_ptr, row_index, col_index);

            if (value) {
              // Increment the weight for this column.
              col_weight++;
              // Index from 1, not 0, in the alist format.
              out << row_index + 1;
              // Append this index to the string.
              temp_string_4 += out.str();
              // Now clear the stringstream.
              out.str("");
              // Add a space too.
              temp_string_4 += " ";
            }
          }

          // Add a newline between columns
          temp_string_4 += "\n";

          // Check if the current column has max weight.
          if (col_weight > max_col_weight) {
            max_col_weight = col_weight;
          }

          // Use stringstream to convert unsigned int to string
          out << col_weight; 
          temp_string_3 += out.str(); // Append the col weight
          out.str("");                // Clear the stringstream
          temp_string_3 += " ";       // Add a space b/w values
        }

        // We're done with these strings; add a final newline
        temp_string_1 += "\n";
        temp_string_3 += "\n";

        // Write out the max column and row weights.
        output_file << max_col_weight << " " << max_row_weight << endl;
        // Write out all of the column weights.
        output_file << temp_string_3;
        // Write out all of the row weights.
        output_file << temp_string_1;
        // Write out the nonzero indices for each column.
        output_file << temp_string_4;
        // Write out the nonzero indices for each row.
        output_file << temp_string_2;
        // Close the file. 
        output_file.close();
      }

      void LDPC_par_chk_mtrx_impl::read_matrix_from_file(string *file) {
        /* This function reads in an alist file and creates the
          corresponding parity check matrix. The format of alist
          files is desribed at:
          http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
        */
        string alist_filename(*file);
        ifstream inputFile;

        // Open the alist file (needs a C-string)
        inputFile.open(alist_filename.c_str());
        if (!inputFile) {
          cout << "There was a problem opening file "
             << alist_filename << " for reading.\n";
          exit(1);
        }

        // First line of file is matrix size: # columns, # rows
        inputFile >> d_n >> d_num_rows; 

        // Now we can allocate memory for the GSL matrix
        d_H_ptr = gsl_matrix_alloc(d_num_rows, d_n);

        // Since the matrix will be sparse, start by filling it with
        // all 0s
        gsl_matrix_set_zero(d_H_ptr);

        // The next few lines in the file are not neccesary in
        // contructing the matrix.
        string tempBuffer;
        unsigned int counter;
        for (counter = 0; counter < 4; counter++) {
          getline(inputFile,tempBuffer);
        }

        // These next lines list the indices for where the 1s are in
        // each column.
        unsigned int column_count = 0;
        string row_number;

        while (column_count < d_n) {
          getline(inputFile,tempBuffer);
          stringstream ss(tempBuffer);

          while (ss >> row_number) {
            int row_i = atoi(row_number.c_str());
            // alist files index starting from 1, not 0, so decrement
            row_i--;
            // set the corresponding matrix element to 1
            gsl_matrix_set(d_H_ptr,row_i,column_count,1);
          }
          column_count++;
        }

        // The subsequent lines in the file list the indices for
        // where the 1s are in the rows, but this is redundant
        // information that we already have. 

        // Close the alist file
        inputFile.close();

        // Length of information word = (# of columns) - (# of rows)
        d_k = d_n - d_num_rows;
       }

      void LDPC_par_chk_mtrx_impl::reduce_H_to_full_rank_matrix() {
        /* This function operates on the parity check matrix H. If H
           is not already full rank, then the function will 
           determine which rows are dependent and remove them. 

           This method, as with the other methods in this class, assumes that the matrix is GF(2), 1s and 0s only. 
        */
        cout << "Original matrix has " << d_num_rows << " rows.\n"; 

        // Create a copy of the original matrix to refer to later
        gsl_matrix *original_matrix = gsl_matrix_alloc(d_num_rows,
                                                       d_n);
        gsl_matrix_memcpy(original_matrix, d_H_ptr); 

        unsigned int rank = 0, index = 0, limit = d_num_rows;
        unsigned int col_index, row_index;

        // Create vectors to save the column and row permutations
        vector<int> column_order;
        vector<int> row_order;
        while (index < d_n) {
          column_order.push_back(index);
          index++;
        }
        
        index = 0;
        while (index < d_num_rows) {
          row_order.push_back(index);
          index++;
        }

        index = 0; 

        while (index < limit) {
          // Flag indicating that row contains a nonzero entry
          bool found_nonzero_entry = false;

          for (col_index = 0; col_index < d_n; col_index++) {
            int value = gsl_matrix_get(d_H_ptr, index, col_index);
            if (value == 1) {
              // Encountered a nonzero entry at (index, col_index)
              found_nonzero_entry = true;

              // Increment the rank by 1
              rank++;

              // Make the entry at (index, index) be 1. This swaps
              // the columns "in-place"
              gsl_matrix_swap_columns(d_H_ptr, col_index, index);

              // keep track of the column swapping
              int temp_value = column_order[index];
              column_order[index] = column_order[col_index];
              column_order[col_index] = temp_value; 

              break;
            }
          }

          if (found_nonzero_entry == true) {
            for (row_index = 0; row_index < d_num_rows;row_index++) {
              if (row_index == index) {
                continue; 
              }
              int value = gsl_matrix_get(d_H_ptr, row_index,index);
              if (value == 1) {
                // Add row # index to row # row_index (few steps)
                // 1. Create a GSL vector
                gsl_vector *temp_vector = gsl_vector_alloc(d_n);
                // 2. Add values from each row, mod 2
                unsigned int col;
                for (col = 0; col < d_n; col++){
                  int val1 = gsl_matrix_get(d_H_ptr,index,col);
                  int val2 = gsl_matrix_get(d_H_ptr, 
                                            row_index,
                                            col);
                  int new_val = (val1 + val2) % 2; 
                  gsl_vector_set(temp_vector, col, new_val);
                }
                // 3. Set this as the new row number row_index 
                //  in H.
                gsl_matrix_set_row(d_H_ptr, 
                                   row_index, 
                                   temp_vector);

                // All of the entries above and below 
                // (index, index) are now 0.
              }
            }
            index++;
          }
          else {
            // Push this row of 0s to the bottom, and move the bottom
            // rows up (sort of a rotation operation). Create a temp
            // matrix to work with. Keep track of row order. 

            // Push row number index to the bottom of the matrix.
            gsl_matrix *temp_matrix=gsl_matrix_alloc(d_num_rows,d_n);
            gsl_matrix_memcpy(temp_matrix, d_H_ptr);
            gsl_vector *temp_vector = gsl_vector_alloc(d_n);
            unsigned int col_index; 
            for (col_index = 0; col_index < d_n; col_index++){
              int value = gsl_matrix_get(d_H_ptr, index,col_index);
              gsl_vector_set(temp_vector, col_index, value);
            } 
            gsl_matrix_set_row(temp_matrix,d_num_rows-1,temp_vector);

            int temp_value1 = row_order[d_num_rows-1];
            row_order[d_num_rows-1]=row_order[index];

            // Now rotate the other rows up
            int value, temp_index = 2;
            while ((d_num_rows - temp_index) >= index) {
              int row_to_move = d_num_rows - temp_index + 1;

              // Keep track of the row shuffling.
              int temp_value2 = row_order[row_to_move-1];
              row_order[row_to_move - 1]=temp_value1;
              temp_value1 = temp_value2;

              for (col_index=0; col_index < d_n; col_index++) {

                value = gsl_matrix_get(d_H_ptr,
                                       row_to_move,
                                       col_index);
                gsl_vector_set(temp_vector, col_index, value);
              }
              gsl_matrix_set_row(temp_matrix,
                                 row_to_move-1,
                                 temp_vector);

              temp_index++;
            }

            // copy temporary array into H
            gsl_matrix_memcpy(d_H_ptr, temp_matrix);

            // Decrease the limit since we just found a row of all 0s
            limit--;
          }
        }

        // Finally, reorder H per the column and row permutations
        unsigned int rows_to_delete = d_num_rows - rank;
        index = 0;
        while (index < rows_to_delete) {
          // The dependent rows will be discared
          row_order.pop_back();
          index++;
        }

        // Set the variables
        d_rank = rank;
        d_num_rows = row_order.size();
        d_k = d_n - d_num_rows;

        gsl_matrix *temp_matrix = gsl_matrix_alloc(d_num_rows,d_n);

        // First, reorder the rows. (Could do columnas first;
        // it doesn't matter.)
        int value;
        for (row_index = 0; row_index < d_num_rows; row_index++) {
          int reference_row = row_order[row_index];
          for (col_index=0; col_index<d_n; col_index++) {
            value = gsl_matrix_get(original_matrix,
                                   reference_row,
                                   col_index);
            gsl_matrix_set(temp_matrix,row_index,col_index,value);
          }
        }

        // Second, reorder the columns.
        for (col_index = 0; col_index < d_n; col_index++) {
          int reference_col = column_order[col_index];
          for (row_index = 0; row_index<d_num_rows; row_index++) {
            value = gsl_matrix_get(original_matrix, 
                                   row_index, 
                                   reference_col);
            gsl_matrix_set(temp_matrix,row_index,col_index,value);
          }
        }

        // Set this as the new H matrix for this object
        d_H_ptr = temp_matrix;

        cout << "Final full rank matrix has " << (*d_H_ptr).size1
             << " rows. (This is the rank of the matrix.)\n";
      }

      void LDPC_par_chk_mtrx_impl::full_rank_to_TABECD_form(
                              bool extra_shuffle) {
        /*  This function performs row/column permutations to bring
          H into approximate upper triangular form via greedy
          upper triangulation method outlined in Modern Coding
          Theory Appendix 1, Section A.2. (See Figure A.11)

          Per email from Dr. Urbanke, author of this textbook, this
          algorithm requires the precondition of H being full rank. 

          TODO Need to add a test to confirm that H is full rank
          before processing, but there isn't a function in GSL that
          returns rank, so for now, just assuming that 
          reduce_H_to_full_rank_matrix() has been called if needed.
        
        */

        unsigned int t = 0; // T matrix will be size t x t
        unsigned int col_index, row_index, value, index, gap = 0;
        bool found_nonsingular_phi = false;

        // Create another copy and call it d_H_ptr_temp. We don't
        // want to save to d_H_ptr until we confirm that the phi
        // matrix is nonsingular.
        gsl_matrix *d_H_ptr_temp = gsl_matrix_alloc(d_num_rows, d_n);
        gsl_matrix_memcpy(d_H_ptr_temp, d_H_ptr);

        // Create copy of original matrix & save as H_t
        gsl_matrix *H_t = gsl_matrix_alloc(d_num_rows, d_n);
        gsl_matrix_memcpy(H_t, d_H_ptr);

        while (t != (d_n-d_k-gap)) {
          // Define H_residual (H_r). This is the matrix between the 
          // bars on page 445 in Modern Coding Theory.
          unsigned int H_r_num_rows = d_n-d_k-gap-t;
          unsigned int H_r_num_cols = d_n-t;
          
          gsl_matrix *H_r = gsl_matrix_alloc(H_r_num_rows,
                                             H_r_num_cols);
          for (row_index=t; row_index < H_r_num_rows+t;row_index++) {
            for (col_index =t;col_index<H_r_num_cols+t;col_index++) {
              value = gsl_matrix_get(d_H_ptr_temp,
                                     row_index,
                                     col_index);
              gsl_matrix_set(H_r, row_index-t, col_index-t, value);
            }
          }

          // Find the weight, or degree, of each column of H_r
          vector<unsigned int> column_degrees;
          unsigned int min_residual_degree = H_r_num_rows;
          for (col_index = 0; col_index < H_r_num_cols;col_index++) {
            unsigned int degree = 0;
            for (row_index=0; row_index < H_r_num_rows;row_index++) {
              value = gsl_matrix_get(H_r,row_index, col_index);
              if (value == 1) {
                degree++;
              }
            }
            column_degrees.push_back(degree);

            // Find the minimum nonzero residual degree (must be +)
            if (degree && (degree < min_residual_degree)) {
              min_residual_degree = degree;
            }
          }

          // Get indices of all of the columns in H_t that have
          // weight equal to the minimum residual degree, then pick 
          // one of them at random to be column c.
          vector<int> indices_in_H_t_with_min_degree;
          for (index = 0; index < column_degrees.size(); index++) {
            if (column_degrees[index] == min_residual_degree) {

              //This is the column number of H_t, not H_r, so add t
              indices_in_H_t_with_min_degree.push_back(index+t);
            }
          }

          // Setup the random number generator
          gsl_rng *rng;
          const gsl_rng_type *T;
          gsl_rng_env_setup();
          T = gsl_rng_default;
          rng = gsl_rng_alloc(T);
          unsigned int num_of_cols = indices_in_H_t_with_min_degree.size();

          // Have to convert the vector to an array for GSL
          int indices_in_H_t_with_min_degree_array[num_of_cols];
          for (index = 0; index < num_of_cols; index++) {
            indices_in_H_t_with_min_degree_array[index] = 
                       indices_in_H_t_with_min_degree[index];
          }

          // Shuffle twice. For some reason, the first and last
          // entries don't ever get shuffled if only shuffled once.
          gsl_ran_shuffle(rng,
                          indices_in_H_t_with_min_degree_array,
                          num_of_cols,
                          sizeof(unsigned int));
          gsl_ran_shuffle(rng,
                          indices_in_H_t_with_min_degree_array,
                          num_of_cols,
                          sizeof(unsigned int));

          unsigned int column_c = indices_in_H_t_with_min_degree_array[0];
          unsigned int row_that_contains_nonzero;

          if (min_residual_degree == 1) {

            // This is the 'extend' case

            // Find the row in column c that contains the 1
            for (row_index=0; row_index < H_r_num_rows;row_index++) {
              unsigned int H_r_column = column_c - t;
              value = gsl_matrix_get(H_r, row_index, H_r_column);
              if (value == 1) {
                row_that_contains_nonzero = row_index + t;
              }
            }

            // Swap column c with column t (book says t+1 but we
            // index from 0, not 1).
            gsl_matrix_swap_columns(H_t, column_c, t);

            // Swap row r with row t (book says t+1 but we index from
            // 0, not 1).
            gsl_matrix_swap_rows(H_t,row_that_contains_nonzero,t);

          }
          else {
            // This is the 'choose' case

            // Find the rows in column c that contains the non-zero
            // entries.
            vector <int> rows_that_contain_nonzero_entries;
            for (row_index=0; row_index < H_r_num_rows;row_index++) {
              unsigned int H_r_column = column_c - t;
              value = gsl_matrix_get(H_r, row_index, H_r_column);
              if (value == 1) {
                row_that_contains_nonzero = row_index + t;
                rows_that_contain_nonzero_entries.push_back(row_that_contains_nonzero);
              }
              else if (value == 0) {
                continue;
              }
              else {
                throw "Error: Found value in the matrix that is not 1 or 0.\n";
              }
            }

            // Swap column c with column t (book says t+1 but we
            // index from 0, not 1.)
            gsl_matrix_swap_columns(H_t, column_c, t);

            // Swap row r1 with row t
            gsl_matrix_swap_rows(H_t,rows_that_contain_nonzero_entries[0], t);

            unsigned int rows_to_move, row_in_H_t; 
            rows_to_move=rows_that_contain_nonzero_entries.size()-1;

            for (index = 1; index <= rows_to_move; index++) {
              // Move the other rows that contain nonzero entries
              // to the bottom of the matrix. We can't just swap 
              // them, because we would be pulling up rows that
              // have been previously pushed down. So, use a 
              // rotation type method.

              row_in_H_t =rows_that_contain_nonzero_entries[index];

              // I'm going to do this with a series of swaps. Say 
              // we had rows 0 1 2 3 4 and we needed to push row 
              // 2 to the bottom. This series of swaps would
              // accomplish this by:
              // swap #1)   0 1 4 3 2
              // swap #2)   0 1 3 4 2

              // start swap with the last row number
              unsigned int row_to_swap_with = (*H_t).size1 - 1;

              while (row_to_swap_with > row_in_H_t) {
                gsl_matrix_swap_rows(H_t,
                                     row_in_H_t,
                                     row_to_swap_with);
                                     row_to_swap_with--;
              }
            }

            // the current H_t is our new candidate
            d_H_ptr_temp = H_t;

            // calculate the new gap value
            gap = gap + (min_residual_degree - 1);
          }
          // increment up t
          t++;

          // clean up memory
          gsl_matrix_free(H_r);
        }

        if (gap == 0) {
          throw "Error in full_rank_to_TABECD_form(). Gap is 0.\n";
        }


        // Phi must be nonsingular if this matrix is to be used in 
        // the encoding algorithm. So, find phi and check.

        // First, define all of the submatrices.
        gsl_matrix_view T = gsl_matrix_submatrix(d_H_ptr_temp,
                                                 0, 
                                                 0, 
                                                 t, 
                                                 t);
        gsl_matrix_view E = gsl_matrix_submatrix(d_H_ptr_temp,
                                                 t,
                                                 0,
                                                 gap,
                                                 d_n - d_k - gap);
         
        gsl_matrix_view A = gsl_matrix_submatrix(d_H_ptr_temp,
                                                 0,
                                                 t,
                                                 t,
                                                 gap);

        gsl_matrix_view C = gsl_matrix_submatrix(d_H_ptr_temp,
                                                 t,
                                                 t,
                                                 gap,
                                                 gap);

        gsl_matrix_view D = gsl_matrix_submatrix(d_H_ptr_temp,
                                                 t,
                                                 d_n-d_k,
                                                 gap,
                                                 d_k);
        gsl_matrix *T_inverse;
        try {
          T_inverse = calc_inverse_mod2(&T.matrix);
        }
        catch (char const *exceptionString) {
          cout << "Initial T inverse not found : "
               << exceptionString;
        }

        gsl_matrix *temp1 = matrix_mult_mod2(&E.matrix, T_inverse);
        gsl_matrix *temp2 = matrix_mult_mod2(temp1, &A.matrix);

        // Solve for phi.
        gsl_matrix *phi = subtract_matrices_mod2(&C.matrix, temp2);
        
        // free some memory
        gsl_matrix_free(temp1);
        gsl_matrix_free(temp2);
        gsl_matrix_free(T_inverse);


        // If phi has at least one nonzero entry, try for inverse.
        if (gsl_matrix_max(phi)) {

          // Test to see if nonsingular phi matrix has been found.
          try {
            gsl_matrix *inverse_phi = calc_inverse_mod2(phi);

            if (gsl_matrix_max(inverse_phi)) {
              // At this point, nonsingular phi matrix was found.
              gsl_matrix_memcpy(d_H_ptr, H_t);
              found_nonsingular_phi = true;
              d_gap = gap;
            }
            else {
              cout << "Error. calc_inverse_mod2 is returning "
                   << "an inverse phi matrix that is all zeros"
                   << ".\n";
              exit(1);
            }
          }
          catch (char const *exceptionString) {
            // cout << "Initial phi matrix is nonsingular: "
            //    << exceptionString;
          }
        }
        else {
          cout << "Initial phi matrix is all zeros.\n";
        }

        // If the C and D submatrices are all zeros, then there is no
        // point in shuffling them around in an attempt to find a
        // nonsingular phi.
        int max_C = gsl_matrix_max(&C.matrix);
        int max_D = gsl_matrix_max(&D.matrix);
        if (!(max_C || max_D)) {

          // Set d_gap = number or rows so it's clear a valid matrix
          // has not been found. 
          d_gap = d_num_rows;

          throw "Error: C and D submatrices are all zeros. It is not possible to find a nonsignular phi matrix\nand therefore encoding is not possible with the current H matrix candidate.\n";
        }

        // We can't look at every row/col permutation possibility
        // because there are (n-t)! column shuffles and g! row
        // shuffles. So, just define a maximum number of iterations
        // to evaluate.
        unsigned int max_iterations = 200, iteration_count = 0,
               num_shuffle_rows = gap, num_shuffle_cols = d_n-t;

        // Create an array of the column numbers that can be shuffled
        int columns_to_shuffle[num_shuffle_cols];
        for (index = 0; index < num_shuffle_cols; index++) {
          columns_to_shuffle[index] = index + t;
        }

        // Create an array of the row numbers that can be shuffled.
        int rows_to_shuffle[num_shuffle_rows];
        for (index = 0; index < num_shuffle_rows; index++ ) {
          rows_to_shuffle[index] = index + t;
        }

        while ((!found_nonsingular_phi) && 
             (iteration_count < max_iterations)) {

          // Allocate memory for the candidate H matrix, temp_H
          gsl_matrix *temp_H = gsl_matrix_alloc(d_num_rows, d_n);
          // Create a copy of H_t matrix. 
          gsl_matrix_memcpy(temp_H, H_t);
          // Setup the random number generator
          const gsl_rng_type *T;
          gsl_rng *rng;
          gsl_rng_env_setup();
          T = gsl_rng_default;
          rng = gsl_rng_alloc(T);

          // Shuffle the array of column/row numbers. Shuffle twice. 
          // For some reason, the first and last entries don't ever get
          // shuffled if you only shuffle once.
          gsl_ran_shuffle(rng,
                          columns_to_shuffle,
                          num_shuffle_cols,
                          sizeof(int));
          gsl_ran_shuffle(rng,
                          columns_to_shuffle,
                          num_shuffle_cols,
                          sizeof(int));
          gsl_ran_shuffle(rng,
                          rows_to_shuffle,
                          num_shuffle_rows,
                          sizeof(int));
          gsl_ran_shuffle(rng,
                          rows_to_shuffle,
                          num_shuffle_rows,
                          sizeof(int));

          // If, from the last time this function
          // full_rank_to_TABECD_form was called, the result was an
          // unusable matrix, we need to do an extra shuffle here.
          // This flag is an argument to this function and is set in
          // the preprocess_for_encoding function.
          if (extra_shuffle) {
            gsl_ran_shuffle(rng,
                            rows_to_shuffle,
                            num_shuffle_rows,
                            sizeof(int));
            gsl_ran_shuffle(rng,
                            columns_to_shuffle,
                            num_shuffle_cols,
                            sizeof(int));
          }

          // Shuffle the rows first.
          for (row_index = t; row_index < d_num_rows; row_index++) {
            unsigned int ref_row = rows_to_shuffle[row_index-t];
            for (col_index = 0; col_index < d_n; col_index++) {
              value = gsl_matrix_get(H_t, ref_row, col_index);
              gsl_matrix_set(temp_H, row_index, col_index, value);
            }
          }
          // Shuffle the columns next.
          for (col_index = t; col_index < d_n; col_index++) {
            unsigned int ref_col = columns_to_shuffle[col_index-t];
            for (row_index= 0; row_index < d_num_rows; row_index++) {
              value = gsl_matrix_get(H_t, row_index, ref_col);
              gsl_matrix_set(temp_H, row_index, col_index, value);
            }
          }

          // Now test this new H matrix candidate.
          gsl_matrix_memcpy(H_t, temp_H);

          gsl_matrix_view newT = gsl_matrix_submatrix(H_t,
                                                      0, 
                                                      0, 
                                                      t, 
                                                      t);
          gsl_matrix_view newE = gsl_matrix_submatrix(H_t,
                                                      t,
                                                      0,
                                                      gap,
                                                      d_n-d_k-gap);
         
          gsl_matrix_view newA = gsl_matrix_submatrix(H_t,
                                                      0,
                                                      t,
                                                      t,
                                                      gap);

          gsl_matrix_view newC = gsl_matrix_submatrix(H_t,
                                                      t,
                                                      t,
                                                      gap,
                                                      gap);

          gsl_matrix *new_T_inverse;
          try {
            new_T_inverse = calc_inverse_mod2(&newT.matrix);
          }
          catch (char const *exceptionString) {
            cout << "On iteration " << iteration_count << ", failed "
                 << "to find T inverse: " << exceptionString;
          }

          temp1 = matrix_mult_mod2(&newE.matrix, new_T_inverse);
          temp2 = matrix_mult_mod2(temp1, &newA.matrix);

          // Solve for phi.
          phi = subtract_matrices_mod2(&newC.matrix, temp2);

          // free some memory
          gsl_matrix_free(temp1);
          gsl_matrix_free(temp2);
          gsl_matrix_free(new_T_inverse);

          // If phi has at least one nonzero entry, try for inverse.
          if (gsl_matrix_max(phi)) {

            // Test to see if nonsingular phi matrix has been found.
            try {
              gsl_matrix *inverse_phi = calc_inverse_mod2(phi);

              if (gsl_matrix_max(inverse_phi)) {
                // At this point, an inverse was found. 
                gsl_matrix_memcpy(d_H_ptr, H_t);
                d_gap = gap;
                found_nonsingular_phi = true;
              }
              else {
                cout << "Error. calc_inverse_mod2 is returning "
                     << "an inverse phi matrix that is all zeros"
                     << ".\n";
                exit(1);
              }

            }
            catch (char const *exceptionString) {
              // cout << "Iteration count " << iteration_count
              //    << ", Error finding phi inverse: " 
              //    << exceptionString;
            }
          }

          iteration_count++;

          // Free memory.
          gsl_matrix_free(temp_H);
        }

        // Free memory
        gsl_matrix_free(H_t);
        // gsl_matrix_free(d_H_ptr_temp); // This causes seg fault (???)

        if (!found_nonsingular_phi) {

          // Set the gap equal to the number or rows, since a valid 
          // phi matrix has not been found. 
          d_gap = d_num_rows;

          throw "Error: Nonsingular phi matrix not found. It is not possible to use the current H matrix candidate for encoding.\n";
        }
      }

      void LDPC_par_chk_mtrx_impl::preprocess_for_encoding() {
        /*  This function will call the full_rank_to_TABECD_form()
          function for num_iterations times. Due to some of the
          random choices made in the Greedy Upper Triangulation
          algorithm, the same initial matrix can result in matrices
          with different size gap values. A lower gap corresponds to
          reduced complexity during the subsquent computations of 
          codewords.

          Thus, we want to call the Greedy algorithm repeatedly,
          looking for the lowest gap. More details in Richardson and
          Urbanke's book: Modern Coding Theory, Appendix A. 

          Afterwards, the submatrices are saved for later use during
          real-time encoding.
        */

        // Setting this at 20 because that seemed to be sufficient
        // during my testing. Feel free to changes this. More
        // iterations = more time but potentially lower gap.
        unsigned int num_iterations = 20;

        // Note the initial parameters for comparison later.
        unsigned int best_gap = d_gap;
        gsl_matrix *best_H = gsl_matrix_alloc(d_num_rows, d_n);

        unsigned int index = 1;

        // More details regarding the flag in the catch block below
        bool extra_shuffle_flag = false;

        if (d_gap != d_num_rows) {
          cout << "First H matrix has a gap of " << d_gap << ".\n";
        }
        else {
          cout << "\nValid H matrix has not been found. Looking for "
               << "a candidate.\n";
        }
        cout << endl;

        while (index <= num_iterations) {
          cout << "Iteration: " << index << "/" << num_iterations 
               << endl;

          try {
            full_rank_to_TABECD_form(extra_shuffle_flag);

            // If we're here, the full_rank_to_TABECD_form
            // function found an acceptable matrix.
            extra_shuffle_flag = false;

            // See if a better H matrix was found (lower gap)
            if (d_gap < best_gap) {

              // This matrix is our new candidate to go with.
              best_gap = d_gap;
              gsl_matrix_memcpy(best_H, d_H_ptr);        

              cout << "  New smaller gap found: " << d_gap << endl;
            }
            else {
              cout << "  Gap found is " << d_gap << ", which is "
                   << "not smaller than the current best candidate"
                   << " matrix, which has a gap of " << best_gap
                   << ".\n";         
            }
          }
          catch (char const *exceptionString){

            cout << "  " << exceptionString;

            // If we're here, then the preprocess_for_encoding()
            // function didn't find an acceptable H candidate matrix.
            // So, we need to do some extra shuffling the next time
            // around, otherwise we'll just keep getting the same bad
            // matrix again and again.
            extra_shuffle_flag = true;
          }
          index++;
        }

        d_gap = best_gap;
        
        if (d_gap == d_num_rows) {
          gsl_matrix_free(best_H);
          gsl_matrix_free(d_H_ptr);
          throw "A valid parity check matrix was not found.\n";
        }
        else {
          // Declare the winner!
          gsl_matrix_memcpy(d_H_ptr, best_H);

          cout << "\nThe best H matrix was found to have a gap of "
               << d_gap << ".\n";
        }
      }

      void LDPC_par_chk_mtrx_impl::set_parameters_for_encoding() {

        // This function defines all of the submatrices that will be
        // needed during encoding. 
        
        unsigned int t = d_num_rows - d_gap; 

        // T submatrix
        gsl_matrix_view T_view = gsl_matrix_submatrix(d_H_ptr,
                                0,
                                0,
                                t,
                                t);
        try {
          d_T_inverse = calc_inverse_mod2(&T_view.matrix);
        }
        catch (char const *exceptionString) {
          cout << "Error in set_parameters_for_encoding while "
               << "looking for inverse T: " << exceptionString
               << "This inverse was already take earlier so there "
               << "should not be an issue...?\n";

          exit(1);
        }

        // E submatrix
        gsl_matrix_view E_view = gsl_matrix_submatrix(d_H_ptr,
                                                      t,
                                                      0,
                                                      d_gap,
                                                      d_n-d_k-d_gap);
        d_E = &E_view.matrix;

        // A submatrix
        gsl_matrix_view A_view = gsl_matrix_submatrix(d_H_ptr,
                                                      0,
                                                      t,
                                                      t,
                                                      d_gap);
        d_A = &A_view.matrix;

        // C submatrix (used to find phi but not during encoding)
        gsl_matrix_view C_view = gsl_matrix_submatrix(d_H_ptr,
                                                      t,
                                                      t,
                                                      d_gap,
                                                      d_gap);

        // These are just temporary matrices used to find phi.
        gsl_matrix *temp1 = matrix_mult_mod2(d_E, d_T_inverse);
        gsl_matrix *temp2 = matrix_mult_mod2(temp1, d_A);

        // Solve for phi.
        gsl_matrix *phi = subtract_matrices_mod2(&C_view.matrix,
                                                 temp2);

        // If phi has at least one nonzero entry, try for inverse.
        if (gsl_matrix_max(phi)) {
          try {
            gsl_matrix *inverse_phi = calc_inverse_mod2(phi);
              
            // At this point, an inverse was found. 
            d_phi_inverse = inverse_phi;

          }
          catch (char const *exceptionString) {

            cout << "Error in set_parameters_for_encoding while "
                 << "finding inverse_phi: " << exceptionString
                 << "This inverse was already take earlier so there "
                 << "should not be an issue...?\n";      
            exit(1);
          }
        }

        // B submatrix
        gsl_matrix_view B_view = gsl_matrix_submatrix(d_H_ptr,
                                                      0,
                                                      t + d_gap,
                                                      t,
                                                      d_n-d_gap-t);
        d_B = &B_view.matrix;

        // D submatrix
        gsl_matrix_view D_view = gsl_matrix_submatrix(d_H_ptr,
                                                      t,
                                                      t + d_gap,
                                                      d_gap,
                                                      d_n-d_gap-t);
        d_D = &D_view.matrix;
      }

      gsl_matrix *LDPC_par_chk_mtrx_impl::subtract_matrices_mod2(
                                               gsl_matrix *matrix1,
                                               gsl_matrix *matrix2) {
        // This function returns ((matrix1 - matrix2) % 2). 

        // Verify that matrix sizes are appropriate
        unsigned int matrix1_rows = (*matrix1).size1;
        unsigned int matrix1_cols = (*matrix1).size2;
        unsigned int matrix2_rows = (*matrix2).size1;
        unsigned int matrix2_cols = (*matrix2).size2;

        if (matrix1_rows != matrix2_rows) {
          cout << "Error in subtract_matrices_mod2. Matrices do not "
               << "have the same number of rows.\n";
          exit(1);
        }
        if (matrix1_cols != matrix2_cols) {
          cout << "Error in subtract_matrices_mod2. Matrices do not "
               << "have the same number of columns.\n";
          exit(1);
        }

        // Allocate memory for the result
        gsl_matrix *result = gsl_matrix_alloc(matrix1_rows,
                                              matrix1_cols);

        // Copy matrix1 into result
        gsl_matrix_memcpy(result, matrix1);

        // Do subtraction. This is not mod 2 yet.
        gsl_matrix_sub(result, matrix2);

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

      gsl_matrix *LDPC_par_chk_mtrx_impl::matrix_mult_mod2(
                                             gsl_matrix *matrix1,
                                             gsl_matrix *matrix2) {
        // Verify that matrix sizes are appropriate
        unsigned int a = (*matrix1).size1;  // # of rows
        unsigned int b = (*matrix1).size2;  // # of columns
        unsigned int c = (*matrix2).size1;  // # of rows
        unsigned int d = (*matrix2).size2;  // # of columns
        if (b != c) {
          cout << "Error in matrix_mult_mod2. Matrix dimensions do"
               << "not allow for matrix multiplication operation:\n"
               << "matrix1 has " << a << " columns and matrix2 has " 
               << b << " rows.\n";
          exit(1);
        }

        // Allocate memory for the result
        gsl_matrix *result = gsl_matrix_alloc(a,d);

        // Perform matrix multiplication. This is not mod 2.
        gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, 
                        matrix1, matrix2, 0.0, result);

        // Take care of mod 2 manually
        unsigned int row_index, col_index;
        unsigned int rows = (*result).size1, 
        cols = (*result).size2;
        for (row_index = 0; row_index < rows; row_index++) {
          for (col_index = 0; col_index < cols; col_index++) {
            int value = gsl_matrix_get(result,
                                       row_index,
                                       col_index);
            int new_value = value % 2;
            gsl_matrix_set(result,
                           row_index,
                           col_index,
                           new_value);
          }
        }
        return result;
      }

      gsl_matrix *LDPC_par_chk_mtrx_impl::calc_inverse_mod2(gsl_matrix *original_matrix) {

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
    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */