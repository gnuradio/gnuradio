/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_RMS_FF_H
#define INCLUDED_BLOCKS_RMS_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief RMS average power
 * \ingroup math_operators_blk
 */
class BLOCKS_API rms_ff : virtual public sync_block
{
public:
    // gr::blocks::rms_ff::sptr
    typedef std::shared_ptr<rms_ff> sptr;

    /*!
     * \brief Make an RMS calc. block.
     * \param alpha gain for running average filter.
     */
    static sptr make(double alpha = 0.0001);

    virtual void set_alpha(double alpha) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_RMS_FF_H */
