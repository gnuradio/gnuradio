/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/rpcserver_booter_aggregator.h>

rpcserver_booter_aggregator::rpcserver_booter_aggregator()
    : d_type(std::string("aggregator")), server(new rpcserver_aggregator())
{
    ;
}

rpcserver_booter_aggregator::~rpcserver_booter_aggregator() { ; }

rpcserver_base* rpcserver_booter_aggregator::i() { return &(*server); }

const std::string& rpcserver_booter_aggregator::type() { return d_type; }

const std::vector<std::string> rpcserver_booter_aggregator::endpoints()
{
    std::vector<std::string> ep;
    ep.push_back(std::string("TODO"));
    return ep;
}

const std::vector<std::string>& rpcserver_booter_aggregator::registeredServers()
{
    return server->registeredServers();
}

rpcserver_aggregator* rpcserver_booter_aggregator::agg() { return &(*server); }
