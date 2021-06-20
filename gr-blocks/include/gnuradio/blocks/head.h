/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_HEAD_H
#define INCLUDED_GR_HEAD_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstddef> // size_t

namespace gr {
namespace blocks {

/*!
 * \brief stop after processing the first N items
 * \ingroup misc_blk
 *
 * \details
 * Useful for building test cases, this block consumes only N items from its input, and
 * copies them to its output, if that is connected.
 *
 * You can hence use this block in series with your sample flow if you want a block
 * downstream of it to be tested with an exact number of input items; or you can put it in
 * parallel to your data path, so that it stops at most one buffer size after the
 * specified number of items has been produced upstream.
 */
class BLOCKS_API head : virtual public sync_block
{
public:
    // gr::blocks::head::sptr
    typedef std::shared_ptr<head> sptr;

    static sptr make(size_t sizeof_stream_item, uint64_t nitems);

    virtual void reset() = 0;
    virtual void set_length(uint64_t nitems) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_HEAD_H */
