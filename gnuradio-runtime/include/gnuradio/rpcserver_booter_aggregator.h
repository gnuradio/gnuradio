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

#ifndef RPCSERVER_BOOTER_AGGREGATOR
#define RPCSERVER_BOOTER_AGGREGATOR

#include <gnuradio/api.h>
#include <gnuradio/rpcserver_booter_base.h>
#include <gnuradio/rpcserver_aggregator.h>
#include <boost/shared_ptr.hpp>
#include <string>

class rpcserver_server;

class GR_RUNTIME_API rpcserver_booter_aggregator :
  public virtual rpcserver_booter_base
{
 public:
  rpcserver_booter_aggregator();
  ~rpcserver_booter_aggregator();

  rpcserver_base* i();
  const std::string& type();
  const std::vector<std::string> endpoints();

  const std::vector<std::string>& registeredServers();

 protected:
  friend class rpcmanager;
  rpcserver_aggregator* agg();

private:
  std::string d_type;
  boost::shared_ptr<rpcserver_aggregator> server;
};

#endif /* RPCSERVER_BOOTER_AGGREGATOR */
