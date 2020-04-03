/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ldpc_G_matrix_H
#define INCLUDED_ldpc_G_matrix_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/fec_mtrx.h>
#include <memory>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Class for storing H or G matrix
 * \ingroup error_coding_blk
 *
 * \details
 * This class stores a matrix variable, specifically
 * either a:
 *
 * 1) Generator matrix, G, in the standard format G = [I P],
 *    where I is an identity matrix and P is the parity
 *    submatrix.
 *
 * or
 *
 * 2) Parity matrix, H, in the standard format H = [P' I],
 *    where P' is the transpose of the parity submatrix and I
 *    is an identity matrix.
 *
 * This variable can used by the ldpc_gen_mtrx_encoder and
 * ldpc_bit_flip_decoder classes.
 */
class FEC_API ldpc_G_matrix : virtual public fec_mtrx,
                              public std::enable_shared_from_this<ldpc_G_matrix>
{
public:
    typedef std::shared_ptr<ldpc_G_matrix> sptr;

    /*!
     * \brief Constructor given alist file
     * \details
     * 1. Reads in the matrix from an alist file
     * 2. Determines if the matrix format is G=[I P] or H=[P' I]
     * 3. Solves for G transpose (will be used during encoding)
     *
     * \param filename Name of an alist file to use. The alist
     *                 format is described at:
     *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
     */
    static sptr make(const std::string filename);

    //! Encode \p inbuffer with LDPC H matrix into \p outbuffer.
    virtual void encode(unsigned char* outbuffer,
                        const unsigned char* inbuffer) const = 0;

    //! Decode \p inbuffer with LDPC H matrix into \p outbuffer.
    virtual void decode(unsigned char* outbuffer,
                        const float* inbuffer,
                        unsigned int frame_size,
                        unsigned int max_iterations) const = 0;

    /*!
     * \brief A pointer to make SWIG work
     *
     * \details
     * SWIG doesn't understand the parent class pointer to this
     * child class for the make function of the
     * ldpc_bit_flip_decoder; it's expecting a pointer to the base
     * class. This returns a shared_from_this instance.
     */
    virtual gr::fec::code::fec_mtrx_sptr get_base_sptr() = 0;
};

} // namespace code
} // namespace fec
} // namespace gr

#endif /* INCLUDED_ldpc_G_matrix_H */
