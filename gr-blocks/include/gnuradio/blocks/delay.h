/* -*- c++ -*- */
/*
 * Copyright 2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_DELAY_H
#define INCLUDED_BLOCKS_DELAY_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>

namespace gr {
namespace blocks {

/*!
 * \brief delay the input by a certain number of samples
 * \ingroup misc_blk
 *
 * Positive delays insert zero items at the beginning of the stream.
 * Negative delays discard items from the stream.
 *
 * You cannot initialize this block with a negative delay,
 * however. That leads to a causality issue with the buffers when
 * they are initialized. If you need to negetively delay one path,
 * then put the positive delay on the other path instead.
 */
class BLOCKS_API delay : virtual public block
{
public:
    // gr::blocks::delay::sptr
    typedef std::shared_ptr<delay> sptr;

    /*!
     * \brief Make a delay block.
     * \param itemsize size of the data items.
     * \param delay number of samples to delay stream (>= 0).
     */
    static sptr make(size_t itemsize, int delay);

    virtual int dly() const = 0;

    /*!
     * \brief Reset the delay.
     * \param d change the delay value. This can be a positive or
     * negative value.
     */
    virtual void set_dly(int d) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_DELAY_H */
