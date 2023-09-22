/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_GR_TPB_THREAD_BODY_H
#define INCLUDED_GR_TPB_THREAD_BODY_H

#include "block_executor.h"
#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/thread/thread.h>

namespace gr {

/*!
 * \brief The body of each thread-per-block thread.
 *
 * One of these is instantiated in its own thread for each block.
 * The constructor turns into the main loop which returns when the
 * block is done or is interrupted.
 */
class GR_RUNTIME_API tpb_thread_body
{
    block_executor d_exec;

public:
    tpb_thread_body(block_sptr block,
                    thread::barrier_sptr start_sync,
                    int max_noutput_items = 100000);
    ~tpb_thread_body();
};

} /* namespace gr */

#endif /* INCLUDED_GR_TPB_THREAD_BODY_H */
