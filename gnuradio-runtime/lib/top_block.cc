/* -*- c++ -*- */
/*
 * Copyright 2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

#include "top_block_impl.h"
#include <gnuradio/top_block.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <gnuradio/high_res_timer.h>
#include <unistd.h>
#include <iostream>

namespace gr {
  top_block_sptr
  make_top_block(const std::string &name)
  {
    return gnuradio::get_initial_sptr
      (new top_block(name));
  }

  top_block::top_block(const std::string &name)
    : hier_block2(name,
                  io_signature::make(0,0,0),
		  io_signature::make(0,0,0))
  {
    d_impl = new top_block_impl(this);
  }

  top_block::~top_block()
  {
    stop();
    wait();

    delete d_impl;
  }

  void
  top_block::start(int max_noutput_items)
  {
#ifdef GNURADIO_HRT_USE_CLOCK_GETTIME
    std::string initial_clock = prefs::singleton()->get_string("PerfCounters", "clock", "thread");
    if(initial_clock.compare("thread") == 0){
        gr::high_res_timer_source = CLOCK_THREAD_CPUTIME_ID;
    } else if(initial_clock.compare("monotonic") == 0){
        gr::high_res_timer_source = CLOCK_MONOTONIC;
    } else {
        throw std::runtime_error("bad argument for PerfCounters.clock!");
    }
#endif

    d_impl->start(max_noutput_items);

    if(prefs::singleton()->get_bool("ControlPort", "on", false)) {
      setup_rpc();
    }
  }

  void
  top_block::stop()
  {
    d_impl->stop();
  }

  void
  top_block::wait()
  {
    d_impl->wait();
  }

  void
  top_block::run(int max_noutput_items)
  {
    start(max_noutput_items);
    wait();
  }

  void
  top_block::lock()
  {
    d_impl->lock();
  }

  void
  top_block::unlock()
  {
    d_impl->unlock();
  }

  std::string
  top_block::edge_list()
  {
    return d_impl->edge_list();
  }

  std::string
  top_block::msg_edge_list()
  {
    return d_impl->msg_edge_list();
  }

  void
  top_block::dump()
  {
    d_impl->dump();
  }

  int
  top_block::max_noutput_items()
  {
    return d_impl->max_noutput_items();
  }

  void
  top_block::set_max_noutput_items(int nmax)
  {
    d_impl->set_max_noutput_items(nmax);
  }

  top_block_sptr
  top_block::to_top_block()
  {
    return cast_to_top_block_sptr(shared_from_this());
  }

  void
  top_block::setup_rpc()
  {
#ifdef GR_CTRLPORT
    if(is_rpc_set())
      return;

    // Triggers
    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_trigger<top_block>(
        alias(), "stop", &top_block::stop,
        "Stop the flowgraph", RPC_PRIVLVL_MIN)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_trigger<top_block>(
        alias(), "lock", &top_block::lock,
        "Lock the flowgraph", RPC_PRIVLVL_MIN)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_trigger<top_block>(
        alias(), "unlock", &top_block::unlock,
        "Unock the flowgraph", RPC_PRIVLVL_MIN)));

    // Getters
    add_rpc_variable(
      rpcbasic_sptr(new rpcbasic_register_get<top_block, int>(
	 alias(), "max noutput_items",
	 &top_block::max_noutput_items,
	 pmt::mp(0), pmt::mp(8192), pmt::mp(8192),
	 "items", "Max number of output items",
	 RPC_PRIVLVL_MIN, DISPNULL)));

    if(prefs::singleton()->get_bool("ControlPort", "edges_list", false)) {
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<top_block, std::string>(
	   alias(), "edge list",
	   &top_block::edge_list,
           pmt::mp(""), pmt::mp(""), pmt::mp(""),
           "edges", "List of edges in the graph",
           RPC_PRIVLVL_MIN, DISPNULL)));
    }

    if(prefs::singleton()->get_bool("ControlPort", "edges_list", false)) {
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<top_block, std::string>(
	   alias(), "msg edges list",
	   &top_block::msg_edge_list,
           pmt::mp(""), pmt::mp(""), pmt::mp(""),
           "msg_edges", "List of msg edges in the graph",
           RPC_PRIVLVL_MIN, DISPNULL)));
    }

#ifdef GNURADIO_HRT_USE_CLOCK_GETTIME
    add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_variable_rw<int>(
        alias(), "perfcounter_clock",
        (int*)&gr::high_res_timer_source,
        pmt::mp(0), pmt::mp(2), pmt::mp(2),
        "clock", "Performance Counters Realtime Clock Type",
        RPC_PRIVLVL_MIN, DISPNULL)));
#endif

    // Setters
    add_rpc_variable(
      rpcbasic_sptr(new rpcbasic_register_set<top_block, int>(
	 alias(), "max noutput_items",
	 &top_block::set_max_noutput_items,
	 pmt::mp(0), pmt::mp(8192), pmt::mp(8192),
	 "items", "Max number of output items",
	 RPC_PRIVLVL_MIN, DISPNULL)));
    rpc_set();
#endif /* GR_CTRLPORT */
  }

} /* namespace gr */
