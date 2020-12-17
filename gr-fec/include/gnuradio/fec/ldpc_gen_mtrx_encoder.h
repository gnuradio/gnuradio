/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H
#define INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/fec/ldpc_G_matrix.h>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief LDPC generator matrix encoder
 * \ingroup error_coding_blk
 *
 * \details
 * A standard encoder class. This method is discussed in many
 * textbooks; one is: Turbo Coding for Satellite and Wireless
 * Communications by Soleymani, Gao, and Vilaipornsawai.
 * Given a generator matrix in systematic form,
 * \f$\mathbf{G}=\left[\mathbf{I}_{k}|\mathbf{P}\right]\f$,
 * where \f$\mathbf{I}_{k}\f$ is the identity matrix and
 * \f$\mathbf{P}\f$ is the parity submatrix, the information
 * word \f$\bar{s}\f$ is encoded into a codeword
 * \f$\overline{x}\f$ via:
 * \f[\overline{x}=\mathbf{G}^{T}\bar{s}\f]
 */
class FEC_API ldpc_gen_mtrx_encoder : virtual public generic_encoder
{
public:
    /*!
     * \brief Build an encoding FEC API object.
     * \param G_obj The ldpc_G_matrix object to use for
     *              encoding.
     */
    static generic_encoder::sptr make(const code::ldpc_G_matrix::sptr G_obj);

    /*!
     * \brief  Sets the uncoded frame size to \p frame_size.
     * \details
     * Sets the uncoded frame size to \p frame_size. If \p
     * frame_size is greater than the value given to the
     * constructor, the frame size will be capped by that initial
     * value and this function will return false. Otherwise, it
     * returns true.
     */
    bool set_frame_size(unsigned int frame_size) override = 0;

    //! Returns the coding rate of this encoder.
    double rate() override = 0;
};
} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_LDPC_GEN_MTRX_ENCODER_H */
