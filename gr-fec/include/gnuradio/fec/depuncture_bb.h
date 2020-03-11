/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_DEPUNCTURE_BB_H
#define INCLUDED_FEC_DEPUNCTURE_BB_H

#include <gnuradio/block.h>
#include <gnuradio/fec/api.h>

namespace gr {
namespace fec {

/*!
 * \brief Depuncture a stream of samples.
 * \ingroup error_coding_blk
 *
 * \details

 * Depuncture a given block of input samples of \p puncsize. The
 * items produced is based on the pattern \p puncpat. Basically,
 * if:
 *
 * \code
 *    k = 0
 *    if _puncpat[i] == 1:
 *       out[i] = input[k++]
 *    else:
 *       out[i] = symbol # default sym=127
 * \endcode
 *
 * This block is designed for unpacked bits - that is, every
 * input sample is a bit, either a 1 or 0. It's possible to use
 * packed bits as symbols, but the depuncturing will be done on
 * the symbol level, not the bit level.
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
 *    output = [a, b, c, 127, e, f, g, h]
 * \endcode
 *
 * The gr.fec Python module provides a read_bitlist function
 * that can turn a string of a puncture pattern into the correct
 * integer form. The pattern of 0xEF could be specified as
 * fec.readbitlist("11101111"). Also, this allows us to use
 * puncsize=len("11101111") to make sure that our sizes are set
 * up correctly for the pattern we want.
 *
 * The fec.extended_decoder takes in the puncture pattern
 * directly as a string and uses the readbitlist inside to do
 * the conversion.
 *
 * The \p delay parameter delays the application of the puncture
 * pattern. This is equivalent to circularly rotating the \p
 * puncpat by \p delay. Note that because of the circular shift,
 * the delay should be between 0 and \p puncsize, but this is
 * not enforced; the effective delay will simply be \p delay mod
 * \p puncsize. A negative value here is ignored.
 */
class FEC_API depuncture_bb : virtual public block
{
public:
    // gr::fec::depuncture_bb::sptr
    typedef boost::shared_ptr<depuncture_bb> sptr;

    /*!
     * \brief Constructs a depuncture block.
     *
     * \param puncsize Size of block of bits to puncture
     * \param puncpat The puncturing pattern
     * \param delay Delayed the puncturing pattern by shifting it
     * \param symbol The symbol to reinsert into the stream (def=127)
     */
    static sptr make(int puncsize, int puncpat, int delay = 0, char symbol = 127);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DEPUNCTURE_BB_H */
