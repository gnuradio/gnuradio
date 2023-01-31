/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2013 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "flat_flowgraph.h"
#include "scheduler_tpb.h"
#include "terminate_handler.h"
#include "top_block_impl.h"
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <gnuradio/top_block.h>

#include <cstdlib>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace gr {

typedef scheduler_sptr (*scheduler_maker)(flat_flowgraph_sptr ffg,
                                          int max_noutput_items,
                                          bool catch_exceptions);


static std::vector<std::tuple<std::string, scheduler_maker>> scheduler_list{
    { "TPB", scheduler_tpb::make }
};
static scheduler_sptr
make_scheduler(flat_flowgraph_sptr ffg, int max_noutput_items, bool catch_exceptions)
{
    static scheduler_maker factory = nullptr;
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "top_block_impl");

    if (!factory) {
        char* environment_var = std::getenv("GR_SCHEDULER");
        if (!environment_var) {
            const auto& [name, fac] = scheduler_list.at(0);
            factory = fac;
            logger->debug("Using default scheduler \"{}\"", name);
        } else {
            for (auto& [name, maker] : scheduler_list) {
                if (name == environment_var) {
                    factory = maker;
                    break;
                }
            }
            if (!factory) {
                const auto& [name, fac] = scheduler_list.at(0);
                factory = fac;
                logger->warn("Invalid GR_SCHEDULER environment variable value \"{:s}\".  "
                             "Using \"{:s}\"",
                             environment_var,
                             name);
            }
        }
    }
    return factory(ffg, max_noutput_items, catch_exceptions);
}

top_block_impl::top_block_impl(top_block* owner, bool catch_exceptions = true)
    : d_owner(owner),
      d_ffg(),
      d_state(IDLE),
      d_lock_count(0),
      d_retry_wait(false),
      d_catch_exceptions(catch_exceptions)
{
    install_terminate_handler();
    gr::configure_default_loggers(d_logger, d_debug_logger, "top_block_impl");
}

top_block_impl::~top_block_impl()
{
    if (d_lock_count) {
        d_logger->error("destroying locked block.");
    }
    // NOTE: Investigate the sensibility of setting a raw pointer to zero at the end of
    // destructor
    d_owner = 0;
}

void top_block_impl::start(int max_noutput_items)
{
    gr::thread::scoped_lock l(d_mutex);

    d_max_noutput_items = max_noutput_items;

    if (d_state != IDLE)
        throw std::runtime_error("top_block::start: top block already running or wait() "
                                 "not called after previous stop()");

    if (d_lock_count > 0)
        throw std::runtime_error("top_block::start: can't start with flow graph locked");

    // Create new flat flow graph by flattening hierarchy
    d_ffg = d_owner->flatten();

    // Validate new simple flow graph and wire it up
    d_ffg->validate();
    d_ffg->setup_connections();

    // Only export perf. counters if ControlPort config param is
    // enabled and if the PerfCounter option 'export' is turned on.
    prefs* p = prefs::singleton();
    if (p->get_bool("ControlPort", "on", false) &&
        p->get_bool("PerfCounters", "export", false))
        d_ffg->enable_pc_rpc();

    d_scheduler = make_scheduler(d_ffg, d_max_noutput_items, d_catch_exceptions);
    d_state = RUNNING;
}

void top_block_impl::stop()
{
    gr::thread::scoped_lock lock(d_mutex);

    if (d_scheduler)
        d_scheduler->stop();

    d_ffg.reset();

    d_state = IDLE;
}

void top_block_impl::wait()
{
    do {
        wait_for_jobs();
        {
            gr::thread::scoped_lock lock(d_mutex);
            if (!d_lock_count) {
                if (d_retry_wait) {
                    d_retry_wait = false;
                    continue;
                }
                d_state = IDLE;
                break;
            }
            d_lock_cond.wait(lock);
        }
    } while (true);
}

void top_block_impl::wait_for_jobs()
{
    if (d_scheduler)
        d_scheduler->wait();
}

// N.B. lock() and unlock() cannot be called from a flow graph
// thread or deadlock will occur when reconfiguration happens
void top_block_impl::lock()
{
    gr::thread::scoped_lock lock(d_mutex);
    if (d_scheduler)
        d_scheduler->stop();
    d_lock_count++;
}

void top_block_impl::unlock()
{
    gr::thread::scoped_lock lock(d_mutex);

    if (d_lock_count <= 0) {
        d_lock_count = 0; // fix it, then complain
        throw std::runtime_error("unpaired unlock() call");
    }

    d_lock_count--;
    if (d_lock_count > 0 || d_state == IDLE) // nothing to do
        return;

    restart();
    d_lock_cond.notify_all();
}

/*
 * restart is called with d_mutex held
 */
void top_block_impl::restart()
{
    wait_for_jobs();

    // Create new simple flow graph
    flat_flowgraph_sptr new_ffg = d_owner->flatten();
    new_ffg->validate();               // check consistency, sanity, etc
    new_ffg->merge_connections(d_ffg); // reuse buffers, etc
    d_ffg = new_ffg;

    // Create a new scheduler to execute it
    d_scheduler = make_scheduler(d_ffg, d_max_noutput_items, d_catch_exceptions);
    d_retry_wait = true;
}

std::string top_block_impl::edge_list()
{
    if (d_ffg)
        return d_ffg->edge_list();
    else
        return "";
}

std::string top_block_impl::msg_edge_list()
{
    if (d_ffg)
        return d_ffg->msg_edge_list();
    else
        return "";
}

void top_block_impl::dump()
{
    if (d_ffg)
        d_ffg->dump();
}

int top_block_impl::max_noutput_items() { return d_max_noutput_items; }

void top_block_impl::set_max_noutput_items(int nmax) { d_max_noutput_items = nmax; }

} /* namespace gr */
