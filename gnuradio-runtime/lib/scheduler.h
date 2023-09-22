/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SCHEDULER_H
#define INCLUDED_GR_SCHEDULER_H

#include "flat_flowgraph.h"
#include <gnuradio/api.h>
#include <gnuradio/block.h>

namespace gr {

class scheduler;
typedef std::shared_ptr<scheduler> scheduler_sptr;

/*!
 * \brief Abstract scheduler that takes a flattened flow graph and
 * runs it.
 *
 * Preconditions: details, buffers and buffer readers have been
 * assigned.
 */
class GR_RUNTIME_API scheduler
{
public:
    /*!
     * \brief Construct a scheduler and begin evaluating the graph.
     *
     * The scheduler will continue running until all blocks
     * report that they are done or the stop method is called.
     */
    scheduler(flat_flowgraph_sptr ffg, int max_noutput_items, bool catch_exceptions);

    virtual ~scheduler();

    scheduler(const scheduler&) = delete;
    scheduler& operator=(const scheduler&) = delete;

    /*!
     * \brief Tell the scheduler to stop executing.
     */
    virtual void stop() = 0;

    /*!
     * \brief Block until the graph is done.
     */
    virtual void wait() = 0;
};

} /* namespace gr */

#endif /* INCLUDED_GR_SCHEDULER_H */
