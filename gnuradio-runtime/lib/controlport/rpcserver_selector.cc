/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gnuradio/rpcserver_booter_aggregator.h>
#include <gnuradio/rpcmanager.h>
#include <gnuradio/rpcserver_selector.h>

bool rpcmanager::make_aggregator(false);
bool rpcmanager::booter_registered(false);
bool rpcmanager::aggregator_registered(false);
std::auto_ptr<rpcserver_booter_base> rpcmanager::boot(0);
std::auto_ptr<rpcserver_booter_aggregator> rpcmanager::aggregator(0);

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
