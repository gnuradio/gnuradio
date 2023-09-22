/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SKIPHEAD_H
#define INCLUDED_GR_SKIPHEAD_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstddef> // size_t

namespace gr {
namespace blocks {

/*!
 * \brief skips the first N items, from then on copies items to the output
 * \ingroup misc_blk
 *
 * \details
 * Useful for building test cases and sources which have metadata
 * or junk at the start
 */
class BLOCKS_API skiphead : virtual public block
{
public:
    // gr::blocks::skiphead::sptr
    typedef std::shared_ptr<skiphead> sptr;

    static sptr make(size_t itemsize, uint64_t nitems_to_skip);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_SKIPHEAD_H */
