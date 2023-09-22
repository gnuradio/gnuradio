/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TOP_BLOCK_IMPL_H
#define INCLUDED_GR_TOP_BLOCK_IMPL_H

#include "scheduler.h"
#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>

namespace gr {

/*!
 *\brief Abstract implementation details of top_block
 * \ingroup internal
 *
 * The actual implementation of top_block. Separate class allows
 * decoupling of changes from dependent classes.
 */
class GR_RUNTIME_API top_block_impl
{
public:
    top_block_impl(top_block* owner, bool catch_exceptions);
    ~top_block_impl();

    // Create and start scheduler threads
    void start(int max_noutput_items = 100000000);

    // Signal scheduler threads to stop
    void stop();

    // Wait for scheduler threads to exit
    void wait();

    // Lock the top block to allow reconfiguration
    void lock();

    // Unlock the top block at end of reconfiguration
    void unlock();

    // Return a string list of edges
    std::string edge_list();

    // Return a string list of msg edges
    std::string msg_edge_list();

    // Dump the flowgraph to stdout
    void dump();

    // Get the number of max noutput_items in the flowgraph
    int max_noutput_items();

    // Set the maximum number of noutput_items in the flowgraph
    void set_max_noutput_items(int nmax);

protected:
    enum tb_state { IDLE, RUNNING };

    top_block* d_owner;
    flat_flowgraph_sptr d_ffg;
    scheduler_sptr d_scheduler;

    gr::thread::mutex d_mutex; // protects d_state and d_lock_count
    tb_state d_state;
    int d_lock_count;
    bool d_retry_wait;
    boost::condition_variable d_lock_cond;
    int d_max_noutput_items;
    bool d_catch_exceptions;

private:
    void restart();
    void wait_for_jobs();

    gr::logger_ptr d_logger, d_debug_logger;
};

} /* namespace gr */

#endif /* INCLUDED_GR_TOP_BLOCK_IMPL_H */
