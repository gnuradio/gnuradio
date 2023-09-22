/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_ROTATOR_CC_H
#define INCLUDED_BLOCKS_ROTATOR_CC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Complex rotator
 * \ingroup math_operators_blk
 *
 * \details
 *
 * Rotates an input complex sequence using a complex exponential in the form of
 * exp(1j * phase_inc * n), where "phase_inc" is a chosen phase increment in
 * radians and "n" is the sample index.
 *
 * Message Ports:
 *
 * - cmd (input):
 *    Receives a PMT dictionary with a command message to set a new phase
 *    increment on the rotator at a specified sample offset. The new increment
 *    must be provided as a PMT double on a key named "inc". The target sample
 *    offset on which to update the phase increment must be given as a PMT
 *    uint64 (with the absolute output item number) on a key named
 *    "offset". Unlike the "inc" key, the "offset" key is optional. When not
 *    provided, the rotator updates its phase increment immediately.
 */
class BLOCKS_API rotator_cc : virtual public sync_block
{
public:
    // gr::blocks::rotator_cc::sptr
    typedef std::shared_ptr<rotator_cc> sptr;

    /*!
     * \brief Make a complex rotator block
     * \param phase_inc rotational velocity
     * \param tag_inc_updates Tag the sample where a phase increment update is
     *                        applied following the reception of a control
     *                        message received via the input message port.
     */
    static sptr make(double phase_inc = 0.0, bool tag_inc_updates = false);

    virtual void set_phase_inc(double phase_inc) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_ROTATOR_CC_H */
