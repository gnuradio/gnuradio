/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UNPACK_K_BITS_BB_H
#define INCLUDED_GR_UNPACK_K_BITS_BB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace blocks {

/*!
 * \brief Converts a byte with k relevant bits to k output bytes with 1 bit in the LSB.
 *
 * This block picks the K least significant bits from a byte, and expands
 * them into K bytes of 0 or 1.
 *
 * Example:
 * k = 4
 * in = [0xf5, 0x08]
 * out = [0,1,0,1, 1,0,0,0]
 * \ingroup byte_operators_blk
 */
class BLOCKS_API unpack_k_bits_bb : virtual public sync_interpolator
{
public:
    // gr::blocks::unpack_k_bits_bb::sptr
    typedef std::shared_ptr<unpack_k_bits_bb> sptr;

    /*!
     * \brief Make an unpack_k_bits block.
     * \param k number of bits to unpack.
     */
    static sptr make(unsigned k);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_UNPACK_K_BITS_BB_H */
