/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_PROBE_RATE_H
#define INCLUDED_BLOCKS_PROBE_RATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief throughput measurement
 * \ingroup measurement_tools_blk
 */
class BLOCKS_API probe_rate : virtual public sync_block
{
public:
    // gr::blocks::probe_rate::sptr
    typedef boost::shared_ptr<probe_rate> sptr;

    /*!
     * \brief Make a throughput measurement block
     * \param itemsize size of each stream item
     * \param update_rate_ms minimum update time in milliseconds
     * \param alpha gain for running average filter
     */
    static sptr
    make(size_t itemsize, double update_rate_ms = 500.0, double alpha = 0.0001);

    virtual void set_alpha(double alpha) = 0;

    virtual double rate() = 0;

    virtual bool start() = 0;
    virtual bool stop() = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PROBE_RATE_H */
