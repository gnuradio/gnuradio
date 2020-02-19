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
 */
class BLOCKS_API rotator_cc : virtual public sync_block
{
public:
    // gr::blocks::rotator_cc::sptr
    typedef boost::shared_ptr<rotator_cc> sptr;

    /*!
     * \brief Make an complex rotator block
     * \param phase_inc rotational velocity
     */
    static sptr make(double phase_inc = 0.0);

    virtual void set_phase_inc(double phase_inc) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_ROTATOR_CC_H */
