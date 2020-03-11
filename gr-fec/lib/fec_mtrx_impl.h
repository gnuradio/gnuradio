/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_fec_mtrx_impl_H
#define INCLUDED_fec_mtrx_impl_H

#include <string>

#include <gnuradio/fec/fec_mtrx.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_randist.h>

namespace gr {
namespace fec {
namespace code {

class fec_mtrx_impl : public fec_mtrx
{
protected:
    //! Constructor
    fec_mtrx_impl();

    //! Codeword length n
    unsigned int d_n;

    //! Information word length k
    unsigned int d_k;

    //! Number of rows in the matrix read in from alist file
    unsigned int d_num_rows;

    //! Number of columns in the matrix read in from alist file
    unsigned int d_num_cols;

    //! GSL matrix structure for the parity check matrix
    matrix_sptr d_H_sptr;

    //! Flag for whether or not the parity bits come first or last
    bool d_par_bits_last;

public:
    //! Returns the parity check matrix H (needed by decoder)
    const gsl_matrix* H() const;

    //! Get the codeword length n
    unsigned int n() const;

    //! Get the information word length k
    unsigned int k() const;

    //! Subtract matrices using mod2 operations
    void
    add_matrices_mod2(gsl_matrix* result, const gsl_matrix*, const gsl_matrix*) const;

    //! Multiply matrices using mod2 operations
    void
    mult_matrices_mod2(gsl_matrix* result, const gsl_matrix*, const gsl_matrix*) const;

    //! Invert a square matrix using mod2 operations
    gsl_matrix* calc_inverse_mod2(const gsl_matrix*) const;

    /*!
     * \brief Get Boolean for whether or not parity bits come first or last
     * \details
     * The decoder will need to know if the parity bits are
     * coming first or last
     */
    bool parity_bits_come_last() const;

    virtual ~fec_mtrx_impl();
};
} // namespace code
} // namespace fec
} // namespace gr

#endif /* INCLUDED_fec_mtrx_impl_H */
