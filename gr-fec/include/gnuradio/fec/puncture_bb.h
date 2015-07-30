/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FEC_PUNCTURE_BB_H
#define INCLUDED_FEC_PUNCTURE_BB_H

#include <gnuradio/fec/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace fec {

    /*!
     * \brief Puncture a stream of unpacked bits.
     * \ingroup error_coding_blk
     *
     * \details
     * Puncture a given block of input samples of \p puncsize. The
     * items produced is based on pattern \p puncpat. Basically, if:
     *
     * \code
     *    k = 0
     *    if _puncpat[i] == 1:
     *       out[k++] = input[i]
     * \endcode
     *
     * This block is designed for unpacked bits - that is, every
     * input sample is a bit, either a 1 or 0. It's possible to use
     * packed bits as symbols, but the puncturing will be done on
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
    class FEC_API puncture_bb : virtual public block
    {
    public:
      // gr::fec::puncture_bb::sptr
      typedef boost::shared_ptr<puncture_bb> sptr;

      /*!
       * \brief Constructs a puncture block for unpacked bits.
       *
       * \param puncsize Size of block of bits to puncture
       * \param puncpat The puncturing pattern
       * \param delay Delayed the puncturing pattern by shifting it
       */
      static sptr make(int puncsize, int puncpat, int delay=0);
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_PUNCTURE_BB_H */
