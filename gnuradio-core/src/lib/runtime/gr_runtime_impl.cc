/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define GR_RUNTIME_IMPL_DEBUG 0

#include <gr_runtime_impl.h>
#include <gr_simple_flowgraph.h>
#include <gr_hier_block2.h>
#include <gr_hier_block2_detail.h>
#include <stdexcept>
#include <iostream>

gr_runtime_impl::gr_runtime_impl(gr_hier_block2_sptr top_block) :
d_running(false),
d_top_block(top_block),
d_sfg(gr_make_simple_flowgraph()),
d_graphs()
{
}

gr_runtime_impl::~gr_runtime_impl()
{
}

void
gr_runtime_impl::start()
{
    if (d_running)
        throw std::runtime_error("already running");
    else
        d_running = true;

    d_sfg->d_detail->reset();
    d_top_block->d_detail->flatten(d_sfg);
    d_sfg->d_detail->validate();
    d_sfg->d_detail->setup_connections();

    d_graphs = d_sfg->d_detail->partition();
    if (GR_RUNTIME_IMPL_DEBUG)
        std::cout << "Flow graph has " << d_graphs.size()
                  << " sub-graphs." << std::endl;

    d_threads.clear();
    for (std::vector<gr_block_vector_t>::iterator p = d_graphs.begin();
         p != d_graphs.end(); p++) {
        gr_scheduler_thread_sptr thread = gr_make_scheduler_thread(*p);
        thread->start();
        d_threads.push_back(thread);
    }
}

void
gr_runtime_impl::stop()
{
    if (!d_running)
        return;

    for (gr_scheduler_thread_viter_t p = d_threads.begin(); 
         p != d_threads.end(); p++)
        (*p)->stop();

    d_running = false;
}

void
gr_runtime_impl::wait()
{
    for (gr_scheduler_thread_viter_t p = d_threads.begin(); 
         p != d_threads.end(); p++) {
        while(1) {
            (*p)->join(NULL);
            if (!(*p)->state() == omni_thread::STATE_TERMINATED)
                break;
        }
    }
}

gr_scheduler_thread_sptr gr_make_scheduler_thread(gr_block_vector_t graph)
{
    return gr_scheduler_thread_sptr(new gr_scheduler_thread(graph));
}

gr_scheduler_thread::gr_scheduler_thread(gr_block_vector_t graph) :
    omni_thread(NULL, PRIORITY_NORMAL),
    d_sts(gr_make_single_threaded_scheduler(graph))
{
}

gr_scheduler_thread::~gr_scheduler_thread()
{
}

void gr_scheduler_thread::start()
{
    start_undetached();
}

void *gr_scheduler_thread::run_undetached(void *arg)
{
    d_sts->run();
    return 0;
}

void gr_scheduler_thread::stop()
{
    d_sts->stop();
}
