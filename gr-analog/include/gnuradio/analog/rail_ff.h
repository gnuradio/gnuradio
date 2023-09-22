/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_RAIL_FF_H
#define INCLUDED_ANALOG_RAIL_FF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief clips input values to min, max
 * \ingroup level_controllers_blk
 */
class ANALOG_API rail_ff : virtual public sync_block
{
public:
    // gr::analog::rail_ff::sptr
    typedef std::shared_ptr<rail_ff> sptr;

    /*!
     * Build a rail block.
     *
     * \param lo the low value to clip to.
     * \param hi the high value to clip to.
     */
    static sptr make(float lo, float hi);

    virtual float lo() const = 0;
    virtual float hi() const = 0;

    virtual void set_lo(float lo) = 0;
    virtual void set_hi(float hi) = 0;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_RAIL_FF_H */
