/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_PUNCTURE_FF_H
#define INCLUDED_FEC_PUNCTURE_FF_H

#include <gnuradio/block.h>
#include <gnuradio/fec/api.h>

namespace gr {
namespace fec {

/*!
 * \brief Puncture a stream of floats.
 * \ingroup error_coding_blk
 *
 * \details
 * For a given block of input samples of \p puncsize, the items
 * produced is based on \p puncpat. Basically, if:
 *
 * \code
 *    k = 0
 *    if _puncpat[i] == 1:
 *       out[k++] = input[i]
 * \endcode
 *
 * This block is designed for floats, generally 1's and -1's. It's
 * possible to use other float values as symbols, but this is not
 * the expected operation.
 *
 * \p puncpat is specified as a 32-bit integer that we can
 * convert into the vector _puncpat used in the algorithm above:
 *
 * \code
 *    _puncpat = [0,...]
 *    for i in puncsize:
 *        _puncpat[i] = puncpat >> (puncsize-1-i)
 * \endcode
 *
 * Example:
 * \code
 *    puncsize = 8
 *    puncpat = 0xEF  -->  [1,1,1,0,1,1,1,1]
 *    input = [a, b, c, d, e, f, g, h]
 *    output = [a, b, c, e, f, g, h]
 * \endcode
 *
 * The gr.fec Python module provides a read_bitlist function
 * that can turn a string of a puncture pattern into the correct
 * integer form. The pattern of 0xEF could be specified as
 * fec.readbitlist("11101111"). Also, this allows us to use
 * puncsize=len("11101111") to make sure that our sizes are set
 * up correctly for the pattern we want.
 *
 * The fec.extended_encoder takes in the puncture pattern
 * directly as a string and uses the readbitlist inside to do
 * the conversion.
 *
 * Note that due to the above concept, the default setting in the
 * extended encoder of '11' translates into no puncturing.
 *
 * The \p delay parameter delays the application of the puncture
 * pattern. This is equivalent to circularly rotating the \p
 * puncpat by \p delay. Note that because of the circular shift,
 * the delay should be between 0 and \p puncsize, but this is
 * not enforced; the effective delay will simply be \p delay mod
 * \p puncsize. A negative value here is ignored.
 */
class FEC_API puncture_ff : virtual public block
{
public:
    // gr::fec::puncture_ff::sptr
    typedef boost::shared_ptr<puncture_ff> sptr;

    /*!
     * \brief Constructs a puncture block for floats.
     *
     * \param puncsize Size of block of bits to puncture
     * \param puncpat The puncturing pattern
     * \param delay Delayed the puncturing pattern by shifting it
     */
    static sptr make(int puncsize, int puncpat, int delay);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_PUNCTURE_FF_H */
