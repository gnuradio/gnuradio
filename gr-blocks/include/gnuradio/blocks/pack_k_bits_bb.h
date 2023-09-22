/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PACK_K_BITS_BB_H
#define INCLUDED_GR_PACK_K_BITS_BB_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
namespace blocks {

/*!
 * \brief Converts a stream of bytes with 1 bit in the LSB to a
 * byte with k relevant bits.
 *
 * This block takes in K bytes at a time, and uses the least significant
 * bit to form a new byte.
 *
 * Example:
 * k = 4
 * in = [0,1,0,1, 0x81,0x00,0x00,0x00]
 * out = [0x05, 0x08]
 *
 * \ingroup byte_operators_blk
 */
class BLOCKS_API pack_k_bits_bb : virtual public sync_decimator
{
public:
    // gr::blocks::pack_k_bits_bb::sptr
    typedef std::shared_ptr<pack_k_bits_bb> sptr;

    /*!
     * \brief Make a pack_k_bits block.
     * \param k number of bits to be packed.
     */
    static sptr make(unsigned k);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PACK_K_BITS_BB_H */
