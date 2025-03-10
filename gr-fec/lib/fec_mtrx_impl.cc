/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fec_mtrx_impl.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace gr {
namespace fec {
namespace code {

// compatibility wrapper. Don't use.
void matrix_free(matrix* x) { gsl_matrix_uchar_free(x); }

matrix_sptr read_matrix_from_file(const std::string filename)
{
    std::ifstream inputFile;
    unsigned int ncols, nrows;

    // Open the alist file (needs a C-string)
    inputFile.open(filename.c_str());
    if (!inputFile) {
        std::stringstream s;
        s << "There was a problem opening file '" << filename << "' for reading.";
        throw std::runtime_error(s.str());
    }

    // First line of file is matrix size: # columns, # rows
    inputFile >> ncols >> nrows;

    // Now we can allocate memory for the GSL matrix
    gsl_matrix_uchar* temp_matrix = gsl_matrix_uchar_alloc(nrows, ncols);
    gsl_matrix_uchar_set_zero(temp_matrix);

    // The next few lines in the file are not necessary in
    // constructing the matrix.
    std::string tempBuffer;
    unsigned int counter;
    for (counter = 0; counter < 4; counter++) {
        getline(inputFile, tempBuffer);
    }

    // These next lines list the indices for where the 1s are in
    // each column.
    unsigned int column_count = 0;
    std::string row_number;

    while (column_count < ncols) {
        getline(inputFile, tempBuffer);
        std::stringstream ss(tempBuffer);

        while (ss >> row_number) {
            int row_i = atoi(row_number.c_str());
            // alist files index starting from 1, not 0, so decrement
            row_i--;
            // set the corresponding matrix element to 1
            gsl_matrix_uchar_set(temp_matrix, row_i, column_count, 1);
        }
        column_count++;
    }

    // Close the alist file
    inputFile.close();

    // Stash the pointer
    matrix_sptr H = matrix_sptr((matrix*)temp_matrix, gsl_matrix_uchar_free);

    return H;
}

void write_matrix_to_file(const std::string filename, matrix_sptr M)
{
    std::ofstream outputfile;

    // Open the output file
    outputfile.open(filename.c_str());
    if (!outputfile) {
        std::stringstream s;
        s << "There was a problem opening file '" << filename << "' for writing.";
        throw std::runtime_error(s.str());
    }

    const unsigned int ncols = M->size2;
    const unsigned int nrows = M->size1;
    std::vector<unsigned int> colweights(ncols, 0);
    std::vector<unsigned int> rowweights(nrows, 0);
    std::stringstream colout;
    std::stringstream rowout;

    for (unsigned int c = 0; c < ncols; c++) {
        for (unsigned int r = 0; r < nrows; r++) {
            auto x = gsl_matrix_uchar_get(M.get(), r, c);
            if (x == 1) {
                colout << (r + 1) << " ";
                colweights[c]++;
            }
        }
        colout << std::endl;
    }

    for (unsigned int r = 0; r < nrows; r++) {
        for (unsigned int c = 0; c < ncols; c++) {
            auto x = gsl_matrix_uchar_get(M.get(), r, c);
            if (x == 1) {
                rowout << (c + 1) << " ";
                rowweights[r]++;
            }
        }
        rowout << std::endl;
    }

    outputfile << ncols << " " << nrows << std::endl;
    outputfile << (*std::max_element(colweights.begin(), colweights.end())) << " "
               << (*std::max_element(rowweights.begin(), rowweights.end())) << std::endl;

    for (const auto& weight : colweights) {
        outputfile << weight << " ";
    }
    outputfile << std::endl;

    for (const auto& weight : rowweights) {
        outputfile << weight << " ";
    }
    outputfile << std::endl;

    outputfile << colout.str() << rowout.str();

    // Close the alist file
    outputfile.close();
}

matrix_sptr generate_G_transpose(matrix_sptr H_obj)
{
    const unsigned int k = H_obj->size1;
    const unsigned int n = H_obj->size2;

    gsl_matrix_uchar* G_transp = gsl_matrix_uchar_alloc(n, k);

    // Grab P' matrix (P' denotes P transposed)
    gsl_matrix_uchar* P_transpose = gsl_matrix_uchar_alloc(n - k, k);
    for (unsigned int row_index = 0; row_index < n - k; row_index++) {
        for (unsigned int col_index = 0; col_index < k; col_index++) {
            gsl_matrix_uchar_set(P_transpose,
                                 row_index,
                                 col_index,
                                 gsl_matrix_uchar_get(H_obj.get(), row_index, col_index));
        }
    }

    // Set G transpose matrix (used for encoding)
    gsl_matrix_uchar_set_zero(G_transp);
    for (unsigned int row_index = 0; row_index < k; row_index++) {
        const auto col_index = row_index;
        gsl_matrix_uchar_set(G_transp, row_index, col_index, 1);
    }
    for (unsigned int row_index = k; row_index < n; row_index++) {
        for (unsigned int col_index = 0; col_index < k; col_index++) {
            gsl_matrix_uchar_set(
                G_transp,
                row_index,
                col_index,
                gsl_matrix_uchar_get(P_transpose, row_index - k, col_index));
        }
    }

    // Stash the pointer
    matrix_sptr G = matrix_sptr(G_transp, gsl_matrix_uchar_free);

    // Free memory
    gsl_matrix_uchar_free(P_transpose);

    return G;
}

matrix_sptr generate_G(matrix_sptr H_obj)
{
    matrix_sptr G_trans = generate_G_transpose(H_obj);

    gsl_matrix_uchar* G = gsl_matrix_uchar_alloc(H_obj->size1, H_obj->size2);

    gsl_matrix_uchar_transpose_memcpy(G, G_trans.get());

    matrix_sptr Gret = matrix_sptr(G, gsl_matrix_uchar_free);
    return Gret;
}

matrix_sptr generate_H(matrix_sptr G_obj)
{
    unsigned int row_index, col_index;

    const unsigned int n = G_obj->size2;
    const unsigned int k = G_obj->size1;

    gsl_matrix_uchar* G_ptr = G_obj.get();
    gsl_matrix_uchar* H_ptr = gsl_matrix_uchar_alloc(n - k, n);

    // Grab P matrix
    gsl_matrix_uchar* P = gsl_matrix_uchar_alloc(k, n - k);
    for (row_index = 0; row_index < k; row_index++) {
        for (col_index = 0; col_index < n - k; col_index++) {
            int value = gsl_matrix_uchar_get(G_ptr, row_index, col_index + k);
            gsl_matrix_uchar_set(P, row_index, col_index, value);
        }
    }

    // Calculate P transpose
    gsl_matrix_uchar* P_transpose = gsl_matrix_uchar_alloc(n - k, k);
    gsl_matrix_uchar_transpose_memcpy(P_transpose, P);

    // Set H matrix. H = [-P' I] but since we are doing mod 2,
    // -P = P, so H = [P' I]
    gsl_matrix_uchar_set_zero(H_ptr);
    for (row_index = 0; row_index < n - k; row_index++) {
        for (col_index = 0; col_index < k; col_index++) {
            int value = gsl_matrix_uchar_get(P_transpose, row_index, col_index);
            gsl_matrix_uchar_set(H_ptr, row_index, col_index, value);
        }
    }

    for (row_index = 0; row_index < n - k; row_index++) {
        col_index = row_index + k;
        gsl_matrix_uchar_set(H_ptr, row_index, col_index, 1);
    }

    // Free memory
    gsl_matrix_uchar_free(P);
    gsl_matrix_uchar_free(P_transpose);

    matrix_sptr H = matrix_sptr(H_ptr, gsl_matrix_uchar_free);
    return H;
}


void print_matrix(const matrix_sptr M, bool numpy)
{
    if (!numpy) {
        for (size_t i = 0; i < M->size1; i++) {
            for (size_t j = 0; j < M->size2; j++) {
                std::cout << gsl_matrix_uchar_get(M.get(), i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cout << "numpy.matrix([ ";
        for (size_t i = 0; i < M->size1; i++) {
            std::cout << "[ ";
            for (size_t j = 0; j < M->size2; j++) {
                std::cout << gsl_matrix_uchar_get(M.get(), i, j) << ", ";
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

const gsl_matrix_uchar* fec_mtrx_impl::H() const
{
    const gsl_matrix_uchar* H_ptr = d_H_sptr.get();
    return H_ptr;
}

unsigned int fec_mtrx_impl::n() const { return d_n; }

unsigned int fec_mtrx_impl::k() const { return d_k; }

void fec_mtrx_impl::add_matrices_mod2(gsl_matrix_uchar* result,
                                      const gsl_matrix_uchar* matrix1,
                                      const gsl_matrix_uchar* matrix2) const
{
    // This function returns ((matrix1 + matrix2) mod 2).

    // Verify that matrix sizes are appropriate
    const unsigned int matrix1_rows = matrix1->size1;
    const unsigned int matrix1_cols = matrix1->size2;
    const unsigned int matrix2_rows = matrix2->size1;
    const unsigned int matrix2_cols = matrix2->size2;

    if (matrix1_rows != matrix2_rows) {
        d_logger.error(
            "add_matrices_mod2: Matrices do not have the same number of rows.");
        throw std::runtime_error("fec_mtrx::add_matrices_mod2: row mismatch");
    }
    if (matrix1_cols != matrix2_cols) {
        d_logger.error(
            "add_matrices_mod2: Matrices do not have the same number of columns.");
        throw std::runtime_error("fec_mtrx::add_matrices_mod2: column mismatch");
    }

    // Take care of mod 2 manually
    for (unsigned int row_index = 0; row_index < matrix1_rows; row_index++) {
        for (unsigned int col_index = 0; col_index < matrix1_cols; col_index++) {
            const auto value1 = gsl_matrix_uchar_get(matrix1, row_index, col_index);
            const auto value2 = gsl_matrix_uchar_get(matrix2, row_index, col_index);
            gsl_matrix_uchar_set(result, row_index, col_index, (value1 + value2) & 1);
        }
    }
}

void fec_mtrx_impl::mult_matrices_mod2(gsl_matrix_uchar* result,
                                       const gsl_matrix_uchar* matrix1,
                                       const gsl_matrix_uchar* matrix2) const
{
    // Verify that matrix sizes are appropriate
    const unsigned int n_rows1 = matrix1->size1; // # of rows
    const unsigned int n_cols1 = matrix1->size2; // # of columns
    const unsigned int n_rows2 = matrix2->size1; // # of rows
    const unsigned int n_cols2 = matrix2->size2; // # of columnsa

    const unsigned int n_rows_res = n_rows1;
    const unsigned int n_cols_res = n_cols2;

    if (n_cols1 != n_rows2) {
        d_logger.error("mult_matrices_mod2: Matrix dimensions do not allow for matrix "
                       "multiplication operation:\n"
                       "matrix1 is {} x {}, and matrix2 is {} x {}",
                       n_rows1,
                       n_cols1,
                       n_rows2,
                       n_cols2);
        throw std::runtime_error("fec_mtrx::mult_matrices_mod2: size mismatch");
    }

    for (unsigned int row = 0; row < n_rows_res; row++) {
        for (unsigned int col = 0; col < n_cols_res; col++) {
            unsigned char sum = 0;
            for (unsigned int idx = 0; idx < n_cols1; idx++) {
                sum += gsl_matrix_uchar_get(matrix1, row, idx) *
                       gsl_matrix_uchar_get(matrix2, idx, col);
            }
            gsl_matrix_uchar_set(result, row, col, sum & 1);
        }
    }
}

gsl_matrix_uchar*
fec_mtrx_impl::calc_inverse_mod2(const gsl_matrix_uchar* original_matrix) const
{
    /* TODO: Don't do this in double / on real-valued matrices, invert in GF2 directly
     *
     * This is both a recipe for slowness and numerical trouble (to invert a matrix in
     * GF2, we invert it in R, then scale it by its determinant, then hope our numerical
     * stability is good enough that we never incorrectly truncate when we cast the
     * floating point numbers to integers, before "remainder to 2"-ing them `%` (note: not
     * the same as mod 2), which is our way to convert it to GF2 values)
     */

    // Let n represent the size of the n x n square matrix
    const unsigned int n = original_matrix->size1;

    // Make a copy of the original matrix, call it matrix_altered.
    // This matrix will be modified by the GSL functions.
    gsl_matrix* matrix_altered = gsl_matrix_alloc(n, n);
    for (unsigned row = 0; row < n; ++row) {
        for (unsigned col = 0; col < n; ++col) {
            gsl_matrix_set(matrix_altered,
                           row,
                           col,
                           gsl_matrix_uchar_get(original_matrix, row, col));
        }
    }

    // In order to find the inverse, GSL must perform a LU
    // decomposition first.
    gsl_permutation* permutation = gsl_permutation_alloc(n);
    int signum;
    gsl_linalg_LU_decomp(matrix_altered, permutation, &signum);

    // Allocate memory to store the matrix inverse
    gsl_matrix* matrix_inverse = gsl_matrix_alloc(n, n);

    // Find matrix inverse. This is not mod2.
    int status = gsl_linalg_LU_invert(matrix_altered, permutation, matrix_inverse);

    if (status) {
        // Inverse not found by GSL functions.
        throw "Error in calc_inverse_mod2(): inverse not found.";
    }

    // Find determinant
    float determinant = gsl_linalg_LU_det(matrix_altered, signum);

    // Multiply the matrix inverse by the determinant.
    gsl_matrix_scale(matrix_inverse, determinant);

    // Take mod 2 of each element in the matrix.
    for (unsigned int row_index = 0; row_index < n; row_index++) {
        for (unsigned int col_index = 0; col_index < n; col_index++) {

            const auto value = gsl_matrix_get(matrix_inverse, row_index, col_index);

            // take care of mod 2
            // FIXME: we cast to int, which will be problematic when something is
            // x.9999998; then we do an fmod on that int; then we do an abs on the float
            // that comes out of that, which we then truncate to int.
            //
            // Not many ways this could be less efficient, and more numerically
            // questionable.
            const int value_cast_as_int = static_cast<int>(value);
            const int temp_value = abs(fmod(value_cast_as_int, 2));

            gsl_matrix_set(matrix_inverse, row_index, col_index, temp_value);
        }
    }
    gsl_matrix_uchar* matrix_inv_gf2 = gsl_matrix_uchar_alloc(n, n);
    unsigned char max_value = 0;
    for (unsigned row = 0; row < n; ++row) {
        for (unsigned col = 0; col < n; ++col) {
            const unsigned char value = gsl_matrix_get(matrix_inverse, row, col);
            max_value = std::max(max_value, value);
            gsl_matrix_uchar_set(matrix_inv_gf2, row, col, value & 1);
        }
    }

    if (!max_value) {
        throw "Error in calc_inverse_mod2(): The matrix inverse found is all zeros.";
    }

    // Verify that the inverse was found by taking matrix
    // product of original_matrix and the inverse, which should
    // equal the identity matrix.
    gsl_matrix_uchar* test = gsl_matrix_uchar_alloc(n, n);
    mult_matrices_mod2(test, original_matrix, matrix_inv_gf2);

    gsl_matrix_uchar* identity = gsl_matrix_uchar_alloc(n, n);
    gsl_matrix_uchar_set_identity(identity);
    // int test_if_equal = gsl_matrix_uchar_equal(identity,test);
    gsl_matrix_uchar_sub(identity, test); // should be null set if equal

    double test_if_not_equal = gsl_matrix_uchar_max(identity);

    if (test_if_not_equal > 0) {
        throw "Error in calc_inverse_mod2(): The matrix inverse found is not valid.";
    }

    return matrix_inv_gf2;
}

bool fec_mtrx_impl::parity_bits_come_last() const { return d_par_bits_last; }

fec_mtrx_impl::~fec_mtrx_impl() {}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */
