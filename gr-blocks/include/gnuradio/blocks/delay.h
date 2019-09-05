/* -*- c++ -*- */
/*
 * Copyright 2007,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
    typedef boost::shared_ptr<delay> sptr;

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
