/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PHASESHIFT_H
#define INCLUDED_PHASESHIFT_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief This block will shift the incoming signal by the specified amount.
 * Shift can be specified in either radians or degrees which is configurable
 * in the constructor.
 *
 * This block functions like a multiply const, but with the const limited to
 * abs() == 1 to provide a constant phase shift.
 * \ingroup misc_blk
 *
 */
class BLOCKS_API phase_shift : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<phase_shift> sptr;

    /*!
     * \brief Create an instance of phase_shift
     */
    static sptr make(float shift, bool is_radians);
    virtual float get_shift() const = 0;
    virtual void set_shift(float new_value) = 0;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_PHASESHIFT_H */
