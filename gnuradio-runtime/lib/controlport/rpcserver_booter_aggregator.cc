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

rpcserver_booter_aggregator::rpcserver_booter_aggregator() :
  d_type(std::string("aggregator")),
  server(new rpcserver_aggregator())
{;}

rpcserver_booter_aggregator::~rpcserver_booter_aggregator()
{;}

rpcserver_base*
rpcserver_booter_aggregator::i()
{
  return &(*server);
}

const std::string&
rpcserver_booter_aggregator::type()
{
  return d_type;
}

const std::vector<std::string>
rpcserver_booter_aggregator::endpoints()
{
  std::vector<std::string> ep;
  ep.push_back(std::string("TODO"));
  return ep;
}

const std::vector<std::string>&
rpcserver_booter_aggregator::registeredServers()
{
  return server->registeredServers();
}

rpcserver_aggregator*
rpcserver_booter_aggregator::agg()
{
  return &(*server);
}
