/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/rpcmanager.h>
#include <gnuradio/rpcserver_booter_aggregator.h>
#include <gnuradio/rpcserver_selector.h>

bool rpcmanager::make_aggregator(false);
bool rpcmanager::booter_registered(false);
bool rpcmanager::aggregator_registered(false);
std::unique_ptr<rpcserver_booter_base> rpcmanager::boot;
std::unique_ptr<rpcserver_booter_aggregator> rpcmanager::aggregator;

#ifdef GR_RPCSERVER_ENABLED
rpcmanager manager_instance;
#endif

#ifdef GR_RPCSERVER_ICE
#error TODO ICE
#endif

#ifdef GR_RPCSERVER_THRIFT
#include <gnuradio/rpcserver_booter_thrift.h>
rpcmanager::rpcserver_booter_register_helper<rpcserver_booter_thrift> boot_thrift;
#endif

#ifdef GR_RPCSERVER_ERLANG
#error TODO ERLANG
#endif

#ifdef GR_RPCSERVER_XMLRPC
#error TODO XMLRPC
#endif
