/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "ldpc_G_matrix_impl.h"
#include <gsl/gsl_matrix_uchar.h>
#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace gr {
namespace fec {
namespace code {

ldpc_G_matrix::sptr ldpc_G_matrix::make(const std::string filename)
{
    return ldpc_G_matrix::sptr(new ldpc_G_matrix_impl(filename));
}

ldpc_G_matrix_impl::ldpc_G_matrix_impl(const std::string filename)
    : fec_mtrx_impl(), d_logger(fmt::format("ldpc_G_matrix {}", filename))
{
    // Read the matrix from a file in alist format
    matrix_sptr x = read_matrix_from_file(filename);
    d_num_cols = x->size2;
    d_num_rows = x->size1;

    // FIXME: totally misled. If we can't read x safely here, then we also can't copy it
    // savely. Don't make that copy. All we do is checking the first d_n×d_n submatrix for
    // identity-matrixness, then transposing and storing the result.
    //
    // Make an actual copy so we guarantee that we're not sharing
    // memory with another class that reads the same alist file.
    gsl_matrix_uchar* G = gsl_matrix_uchar_alloc(d_num_rows, d_num_cols);
    gsl_matrix_uchar_memcpy(G, x.get());


    // First, check if we have a generator matrix G in systematic
    // form, G = [I P], where I is a k x k identity matrix and P
    // is the parity submatrix.

    // Length of codeword = # of columns of generator matrix
    d_n = d_num_cols;
    // Length of information word = # of rows of generator matrix
    d_k = d_num_rows;

    gsl_matrix_uchar* I_test = gsl_matrix_uchar_alloc(d_k, d_k);
    gsl_matrix_uchar* identity = gsl_matrix_uchar_alloc(d_k, d_k);
    gsl_matrix_uchar_set_identity(identity);

    // FIXME: this should be simply a diagonal == 1, else == 0 check up to [d_k, d_k]
    for (unsigned int row_index = 0; row_index < d_k; row_index++) {
        for (unsigned int col_index = 0; col_index < d_k; col_index++) {
            gsl_matrix_uchar_set(I_test,
                                 row_index,
                                 col_index,
                                 gsl_matrix_uchar_get(G, row_index, col_index));
        }
    }

    // Check if the identity matrix exists in the right spot.
    // int test_if_equal = gsl_matrix_uchar_equal(identity, I_test);
    gsl_matrix_uchar_sub(identity, I_test); // should be null set if equal
    double test_if_not_equal = gsl_matrix_uchar_max(identity);

    // Free memory
    gsl_matrix_uchar_free(identity);
    gsl_matrix_uchar_free(I_test);

    // if(!test_if_equal) {
    if (test_if_not_equal > 0) {
        d_logger.error("Error in ldpc_G_matrix_impl constructor. It appears "
                       "that the given alist file did not contain either a "
                       "valid parity check matrix of the form H = [P' I] or "
                       "a generator matrix of the form G = [I P].");
        throw std::runtime_error("ldpc_G_matrix: Bad matrix definition");
    }

    // Our G matrix is verified as correct, now convert it to the
    // parity check matrix.

    // Grab P matrix
    gsl_matrix_uchar* P = gsl_matrix_uchar_alloc(d_k, d_n - d_k);
    // TODO: check whether this is just a submatrix assignment and can be done in gsl
    // FIXME: actually, no, don't do that at all but directly transpose instead of first
    // memcpy for immediate transpose. Instead, just simply read the right elements into
    // H_ptr below
    for (unsigned int row_index = 0; row_index < d_k; row_index++) {
        for (unsigned int col_index = 0; col_index < d_n - d_k; col_index++) {
            gsl_matrix_uchar_set(P,
                                 row_index,
                                 col_index,
                                 gsl_matrix_uchar_get(G, row_index, col_index + d_k));
        }
    }

    // Calculate P transpose
    gsl_matrix_uchar* P_transpose = gsl_matrix_uchar_alloc(d_n - d_k, d_k);
    gsl_matrix_uchar_transpose_memcpy(P_transpose, P);

    // Set H matrix. H = [-P' I] but since we are doing mod 2,
    // -P = P, so H = [P' I]
    gsl_matrix_uchar* H_ptr = gsl_matrix_uchar_alloc(d_n - d_k, d_n);
    gsl_matrix_uchar_set_zero(H_ptr);
    for (unsigned int row_index = 0; row_index < d_n - d_k; row_index++) {
        for (unsigned int col_index = 0; col_index < d_k; col_index++) {
            int value = gsl_matrix_uchar_get(P_transpose, row_index, col_index);
            // FIXME: this could – without hurting legibility – just directly read the
            // correct values, transposing on the way
            gsl_matrix_uchar_set(H_ptr, row_index, col_index, value);
        }
    }

    for (unsigned int row_index = 0; row_index < (d_n - d_k); row_index++) {
        const unsigned int col_index = row_index + d_k;
        gsl_matrix_uchar_set(H_ptr, row_index, col_index, 1);
    }

    // Calculate G transpose (used for encoding)
    d_G_transp_ptr = gsl_matrix_uchar_alloc(d_n, d_k);
    gsl_matrix_uchar_transpose_memcpy(d_G_transp_ptr, G);

    d_H_sptr = matrix_sptr(H_ptr, gsl_matrix_uchar_free);

    // Free memory
    gsl_matrix_uchar_free(P);
    gsl_matrix_uchar_free(P_transpose);
    gsl_matrix_uchar_free(G);
}


const gsl_matrix_uchar* ldpc_G_matrix_impl::G_transpose() const
{
    const gsl_matrix_uchar* G_trans_ptr = d_G_transp_ptr;
    return G_trans_ptr;
}

void ldpc_G_matrix_impl::encode(unsigned char* outbuffer,
                                const unsigned char* inbuffer) const
{

    unsigned int index, k = d_k, n = d_n;
    gsl_matrix_uchar* s = gsl_matrix_uchar_alloc(k, 1);
    for (index = 0; index < k; index++) {
        double value = static_cast<double>(inbuffer[index]);
        gsl_matrix_uchar_set(s, index, 0, value);
    }

    // Simple matrix multiplication to get codeword
    gsl_matrix_uchar* codeword = gsl_matrix_uchar_alloc(G_transpose()->size1, s->size2);
    mult_matrices_mod2(codeword, G_transpose(), s);

    // Output
    for (index = 0; index < n; index++) {
        outbuffer[index] = gsl_matrix_uchar_get(codeword, index, 0);
    }

    // Free memory
    gsl_matrix_uchar_free(s);
    gsl_matrix_uchar_free(codeword);
}


void ldpc_G_matrix_impl::decode(unsigned char* outbuffer,
                                const float* inbuffer,
                                unsigned int frame_size,
                                unsigned int max_iterations) const
{
    gsl_matrix_uchar* x = gsl_matrix_uchar_alloc(d_n, 1);
    for (unsigned int index = 0; index < d_n; index++) {
        gsl_matrix_uchar_set(x, index, 0, inbuffer[index] > 0);
    }

    // Initialize counter
    unsigned int count = 0;

    // Calculate syndrome
    gsl_matrix_uchar* syndrome = gsl_matrix_uchar_alloc(H()->size1, x->size2);
    mult_matrices_mod2(syndrome, H(), x);

    // Flag for finding a valid codeword
    // If the syndrome is all 0s, then codeword is valid and we
    // don't need to loop; we're done.
    bool found_word = gsl_matrix_uchar_isnull(syndrome);

    // Loop until valid codeword is found, or max number of
    // iterations is reached, whichever comes first
    while ((count < max_iterations) && !found_word) {
        // For each of the n bits in the codeword, determine how
        // many of the unsatisfied parity checks involve that bit.
        // To do this, first find the nonzero entries in the
        // syndrome. The entry numbers correspond to the rows of
        // interest in H.
        std::vector<int> rows_of_interest_in_H;
        for (unsigned int index = 0; index < syndrome->size1; index++) {
            if (gsl_matrix_uchar_get(syndrome, index, 0)) {
                rows_of_interest_in_H.push_back(index);
            }
        }

        // Second, for each bit, determine how many of the
        // unsatisfied parity checks involve this bit and store
        // the count.
        std::vector<int> counts(d_n, 0);
        for (unsigned int i = 0; i < rows_of_interest_in_H.size(); i++) {
            unsigned int row_num = rows_of_interest_in_H[i];
            for (unsigned int col_num = 0; col_num < d_n; col_num++) {
                auto value = gsl_matrix_uchar_get(H(), row_num, col_num);
                if (value) {
                    counts[col_num] += 1;
                }
            }
        }

        // Next, determine which bit(s) is associated with the most
        // unsatisfied parity checks, and flip it/them.
        const auto max = *std::max_element(counts.begin(), counts.end());

        for (unsigned int index = 0; index < d_n; index++) {
            if (counts[index] == max) {
                // invert:
                gsl_matrix_uchar_set(x, index, 0, gsl_matrix_uchar_get(x, index, 0) ^ 1);
            }
        }

        // Check the syndrome; see if valid codeword has been found
        mult_matrices_mod2(syndrome, H(), x);
        if (gsl_matrix_uchar_isnull(syndrome)) {
            found_word = true;
            break;
        }
        count++;
    }

    // Extract the info word and assign to output. This will
    // happen regardless of if a valid codeword was found.
    if (parity_bits_come_last()) {
        for (unsigned int index = 0; index < frame_size; index++) {
            outbuffer[index] = gsl_matrix_uchar_get(x, index, 0);
        }
    } else {
        for (unsigned int index = 0; index < frame_size; index++) {
            outbuffer[index] = gsl_matrix_uchar_get(x, index + d_n - frame_size, 0);
        }
    }

    // Free memory
    gsl_matrix_uchar_free(syndrome);
    gsl_matrix_uchar_free(x);
}

gr::fec::code::fec_mtrx_sptr ldpc_G_matrix_impl::get_base_sptr()
{
    return shared_from_this();
}

ldpc_G_matrix_impl::~ldpc_G_matrix_impl()
{
    // Call the gsl_matrix_uchar_free function to free memory.
    gsl_matrix_uchar_free(d_G_transp_ptr);
}
} /* namespace code */
} /* namespace fec */
} /* namespace gr */
