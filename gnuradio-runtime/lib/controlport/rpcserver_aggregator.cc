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

#include <gnuradio/rpcserver_aggregator.h>
#include <gnuradio/rpcserver_booter_base.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

rpcserver_aggregator::rpcserver_aggregator()
  : d_type(std::string("aggregator"))
{;}

rpcserver_aggregator::~rpcserver_aggregator()
{;}

const std::string&
rpcserver_aggregator::type()
{
  return d_type;
}

const std::vector<std::string>&
rpcserver_aggregator::registeredServers()
{
  return d_registeredServers;
}

void
rpcserver_aggregator::registerConfigureCallback(const std::string &id,
						const configureCallback_t callback)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		registerConfigureCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, configureCallback_t>(id, callback));
}

void
rpcserver_aggregator::unregisterConfigureCallback(const std::string &id)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		unregisterConfigureCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, configureCallback_t>(id));
}

void
rpcserver_aggregator::registerQueryCallback(const std::string &id, const queryCallback_t callback)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		registerQueryCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, queryCallback_t>(id, callback));
}

void
rpcserver_aggregator::unregisterQueryCallback(const std::string &id)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		unregisterQueryCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, queryCallback_t>(id));
}



void
rpcserver_aggregator::registerHandlerCallback(const std::string &id,
                                              const handlerCallback_t callback)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		registerHandlerCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, handlerCallback_t>(id, callback));
}

void
rpcserver_aggregator::unregisterHandlerCallback(const std::string &id)
{
  std::for_each(d_serverlist.begin(), d_serverlist.end(),
		unregisterHandlerCallback_f<rpcmanager_base::rpcserver_booter_base_sptr, handlerCallback_t>(id));
}



void
rpcserver_aggregator::registerServer(rpcmanager_base::rpcserver_booter_base_sptr server)
{
  std::vector<std::string>::iterator it(std::find(d_registeredServers.begin(),
						  d_registeredServers.end(),
						  server->type()));
  if(it != d_registeredServers.end()) {
    d_serverlist.push_back(server);
    d_registeredServers.push_back(server->type());
  }
  else {
    std::stringstream s;
    s << "rpcserver_aggregator::registerServer: server of type "
      << server->type() << " already registered" << std::endl;
    throw std::runtime_error(s.str().c_str());
  }
}
