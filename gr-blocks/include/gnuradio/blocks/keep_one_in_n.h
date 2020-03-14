/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_KEEP_ONE_IN_N_H
#define INCLUDED_BLOCKS_KEEP_ONE_IN_N_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief decimate a stream, keeping the last item out of every \p n.
 * \ingroup stream_operators_blk
 */
class BLOCKS_API keep_one_in_n : virtual public block
{
public:
    // gr::blocks::keep_one_in_n::sptr
    typedef std::shared_ptr<keep_one_in_n> sptr;

    /*!
     * Make a keep one in n block.
     *
     * \param itemsize stream itemsize
     * \param n block size in items
     */
    static sptr make(size_t itemsize, int n);

    virtual void set_n(int n) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_KEEP_ONE_IN_N_H */
