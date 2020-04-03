/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ldpc_H_matrix_H
#define INCLUDED_ldpc_H_matrix_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/fec_mtrx.h>
#include <memory>

namespace gr {
namespace fec {
namespace code {
/*!
 * \brief Parity check matrix in Richardson/Urbanke format
 * \ingroup error_coding_blk
 *
 * \details
 * This class stores a matrix for use with the
 * ldpc_encoder class. It must be of the specific format
 * described by Richardson and Urbanke in Appendix A of their
 * book: Modern Coding Theory (ISBN 978-0-521-85229-6). The
 * form is:
 * \f[\left[\begin{array}{ccc} T & A & B\\ E & C & D \end{array}\right]\f]
 * This class can be used with the ldpc_bit_flip_decoder.
 *
 * To convert a parity check matrix to this format, use the
 * python functions in:
 * /lib/python2.7/dist-packages/gnuradio/fec/LDPC/Generate_LDPC_matrix.py.
 */
class FEC_API ldpc_H_matrix : virtual public fec_mtrx,
                              public std::enable_shared_from_this<ldpc_H_matrix>
{
public:
    typedef std::shared_ptr<ldpc_H_matrix> sptr;

    /*!
     * \brief Constructor given alist file and gap
     * \param filename Name of an alist file to use. The alist
     *                 format is described at:
     *                 http://www.inference.phy.cam.ac.uk/mackay/codes/alist.html
     * \param gap A property of the matrix being used. For alist
     *            files distributed with GNU Radio, this value
     *            is specified in the alist filename. The gap is
     *            found during the matrix preprocessing
     *            algorithm. It is equal to the number of rows in
     *            submatrices E, C and D.
     */
    static sptr make(const std::string filename, unsigned int gap);

    //! Encode \p inbuffer with LDPC H matrix into \p outbuffer.
    virtual void encode(unsigned char* outbuffer,
                        const unsigned char* inbuffer) const = 0;

    //! Decode \p inbuffer with LDPC H matrix into \p outbuffer.
    virtual void decode(unsigned char* outbuffer,
                        const float* inbuffer,
                        unsigned int frame_size,
                        unsigned int max_iterations) const = 0;

    //! Get the codeword length n
    //  Handled in fec_mtrx parent class.
    virtual unsigned int n() const = 0;

    //! Get the information word length k
    //  Handled in fec_mtrx parent class.
    virtual unsigned int k() const = 0;

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

#endif /* INCLUDED_ldpc_H_matrix_H */
