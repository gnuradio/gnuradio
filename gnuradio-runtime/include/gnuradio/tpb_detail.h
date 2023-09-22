/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TPB_DETAIL_H
#define INCLUDED_GR_TPB_DETAIL_H

#include <gnuradio/api.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>
#include <deque>

namespace gr {

class block_detail;

/*!
 * \brief used by thread-per-block scheduler
 */
struct GR_RUNTIME_API tpb_detail {
    gr::thread::mutex mutex; //< protects all vars
    bool input_changed;
    gr::thread::condition_variable input_cond;
    bool output_changed;
    gr::thread::condition_variable output_cond;

public:
    tpb_detail() : input_changed(false), output_changed(false) {}

    //! Called by us to tell all our upstream blocks that their output
    //! may have changed.
    void notify_upstream(block_detail* d);

    //! Called by us to tell all our downstream blocks that their
    //! input may have changed.
    void notify_downstream(block_detail* d);

    //! Called by us to notify both upstream and downstream
    void notify_neighbors(block_detail* d);

    //! Called by pmt msg posters
    void notify_msg()
    {
        gr::thread::scoped_lock guard(mutex);
        input_changed = true;
        input_cond.notify_one();
        output_changed = true;
        output_cond.notify_one();
    }

    //! Called by us
    void clear_changed()
    {
        gr::thread::scoped_lock guard(mutex);
        input_changed = false;
        output_changed = false;
    }

private:
    //! Used by notify_downstream
    void set_input_changed()
    {
        gr::thread::scoped_lock guard(mutex);
        input_changed = true;
        input_cond.notify_one();
    }

    //! Used by notify_upstream
    void set_output_changed()
    {
        gr::thread::scoped_lock guard(mutex);
        output_changed = true;
        output_cond.notify_one();
    }
};

} /* namespace gr */

#endif /* INCLUDED_GR_TPB_DETAIL_H */
