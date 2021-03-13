/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2012,2013 Free Software Foundation, Inc.
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
 * \brief copies the first N items to the output then signals done
 * \ingroup misc_blk
 *
 * \details
 * Useful for building test cases
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
