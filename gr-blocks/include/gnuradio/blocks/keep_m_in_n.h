/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_KEEP_M_IN_N_H
#define INCLUDED_BLOCKS_KEEP_M_IN_N_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief decimate a stream, keeping the first \p m items out of every \p n starting after
 * \p offset items. \ingroup stream_operators_blk
 */
class BLOCKS_API keep_m_in_n : virtual public block
{
public:
    // gr::blocks::keep_m_in_n::sptr
    typedef std::shared_ptr<keep_m_in_n> sptr;

    /*!
     * Make a keep m in n block.
     *
     * \param itemsize stream itemsize
     * \param m number of items to take in block of \p n items
     * \param n block size in items
     * \param offset initial item offset into the stream
     */
    static sptr make(size_t itemsize, int m, int n, int offset);

    virtual void set_m(int m) = 0;
    virtual void set_n(int n) = 0;
    virtual void set_offset(int offset) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_KEEP_M_IN_N_H */
