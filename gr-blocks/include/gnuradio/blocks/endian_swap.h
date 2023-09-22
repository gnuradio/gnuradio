/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ENDIAN_SWAP_H
#define INCLUDED_GR_ENDIAN_SWAP_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Convert stream of items into their byte swapped version
 * \ingroup stream_operators_blk
 */
class BLOCKS_API endian_swap : virtual public sync_block
{
public:
    // gr::blocks::endian_swap::sptr
    typedef std::shared_ptr<endian_swap> sptr;

    /*!
     * Make an endian swap block.
     *
     * \param item_size_bytes number of bytes per item, 1=no-op,
     *        2=uint16_t, 4=uint32_t, 8=uint64_t
     */
    static sptr make(size_t item_size_bytes = 1);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ENDIAN_SWAP_H */
