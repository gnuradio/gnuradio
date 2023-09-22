/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_REPEAT_H
#define INCLUDED_BLOCKS_REPEAT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
namespace blocks {

/*!
 * \brief repeat each input \p repeat times
 * \ingroup stream_operators_blk
 *
 * Message Ports:
 *   * interpolation (in):
 *      Takes a pmt_pair(pmt::mp("interpolation"), pmt_long interp), setting the
 * interpolation to interp.
 */
class BLOCKS_API repeat : virtual public block
{
public:
    // gr::blocks::repeat::sptr
    typedef std::shared_ptr<repeat> sptr;

    /*!
     * Make a repeat block.
     *
     * \param itemsize stream itemsize
     * \param repeat number of times to repeat the input
     */
    static sptr make(size_t itemsize, int repeat);

    /*!
     * \brief Return current interpolation
     */
    virtual int interpolation() const = 0;

    /*!
     * \brief sets the interpolation
     *
     * Call this method in a callback to adjust the interpolation at run time.
     *
     * \param interp interpolation to be set
     */
    virtual void set_interpolation(int interp) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_REPEAT_H */
