/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_STRETCH_FF_H
#define INCLUDED_GR_STRETCH_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief adjust y-range of an input vector by mapping to range
 * (max-of-input, stipulated-min). Primarily for spectral
 * signature matching by normalizing spectrum dynamic ranges.
 * \ingroup stream_operators_blk
 */
class BLOCKS_API stretch_ff : virtual public sync_block
{
public:
    // gr::blocks::stretch_ff::sptr
    typedef std::shared_ptr<stretch_ff> sptr;

    /*!
     * \brief Make a stretch block.
     *
     * \param lo Set low value for range.
     * \param vlen vector length of input stream.
     */
    static sptr make(float lo, size_t vlen = 1);

    virtual float lo() const = 0;
    virtual void set_lo(float lo) = 0;
    virtual size_t vlen() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_STRETCH_FF_H */
