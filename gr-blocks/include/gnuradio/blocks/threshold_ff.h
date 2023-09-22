/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_THRESHOLD_FF_H
#define INCLUDED_GR_THRESHOLD_FF_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Output a 1 or zero based on a threshold value.
 * \ingroup level_controllers_blk
 *
 * \details
 * Test the incoming signal against a threshold. If the signal
 * exceeds the \p hi value, it will output a 1 until the signal
 * falls below the \p lo value.
 */
class BLOCKS_API threshold_ff : virtual public sync_block
{
public:
    // gr::blocks::threshold_ff::sptr
    typedef std::shared_ptr<threshold_ff> sptr;

    /* \brief Create a threadshold block.
     * \param lo Threshold input signal needs to drop below to
     *           change state to 0.
     * \param hi Threshold input signal needs to rise above to
     *           change state to 1.
     * \param initial_state Initial state of the block (0 or 1).
     */
    static sptr make(float lo, float hi, float initial_state = 0);

    virtual float lo() const = 0;
    virtual void set_lo(float lo) = 0;
    virtual float hi() const = 0;
    virtual void set_hi(float hi) = 0;
    virtual float last_state() const = 0;
    virtual void set_last_state(float last_state) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_THRESHOLD_FF_H */
