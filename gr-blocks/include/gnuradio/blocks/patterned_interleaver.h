/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H
#define INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief Interleave items based on the provided vector \p pattern.
 * \ingroup stream_operators_blk
 */
class BLOCKS_API patterned_interleaver : virtual public block
{
public:
    typedef std::shared_ptr<patterned_interleaver> sptr;

    /*!
     * Make a patterned interleaver block.
     *
     * \param itemsize stream itemsize
     * \param pattern vector that represents the interleaving pattern
     */
    static sptr make(size_t itemsize, std::vector<int> pattern);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H */
