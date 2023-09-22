/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SCHEDULER_TPB_H
#define INCLUDED_GR_SCHEDULER_TPB_H

#include "scheduler.h"
#include <gnuradio/api.h>
#include <gnuradio/thread/thread_group.h>

namespace gr {

/*!
 * \brief Concrete scheduler that uses a kernel thread-per-block
 */
class GR_RUNTIME_API scheduler_tpb : public scheduler
{
    gr::thread::thread_group d_threads;

protected:
    /*!
     * \brief Construct a scheduler and begin evaluating the graph.
     *
     * The scheduler will continue running until all blocks until they
     * report that they are done or the stop method is called.
     */
    scheduler_tpb(flat_flowgraph_sptr ffg, int max_noutput_items, bool catch_exceptions);

public:
    static scheduler_sptr make(flat_flowgraph_sptr ffg,
                               int max_noutput_items = 100000,
                               bool catch_exceptions = true);

    ~scheduler_tpb() override;

    /*!
     * \brief Tell the scheduler to stop executing.
     */
    void stop() override;

    /*!
     * \brief Block until the graph is done.
     */
    void wait() override;
};

} /* namespace gr */

#endif /* INCLUDED_GR_SCHEDULER_TPB_H */
