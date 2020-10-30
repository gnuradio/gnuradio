/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCSERVER_BOOTER_AGGREGATOR
#define RPCSERVER_BOOTER_AGGREGATOR

#include <gnuradio/api.h>
#include <gnuradio/rpcserver_aggregator.h>
#include <gnuradio/rpcserver_booter_base.h>
#include <memory>
#include <string>

class rpcserver_server;

class GR_RUNTIME_API rpcserver_booter_aggregator : public virtual rpcserver_booter_base
{
public:
    rpcserver_booter_aggregator();
    ~rpcserver_booter_aggregator() override;

    rpcserver_base* i() override;
    const std::string& type() override;
    const std::vector<std::string> endpoints() override;

    const std::vector<std::string>& registeredServers();

protected:
    friend class rpcmanager;
    rpcserver_aggregator* agg();

private:
    std::string d_type;
    std::shared_ptr<rpcserver_aggregator> server;
};

#endif /* RPCSERVER_BOOTER_AGGREGATOR */
