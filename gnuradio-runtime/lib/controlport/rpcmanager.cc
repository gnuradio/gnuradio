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
#include <cassert>
#include <memory>
#include <stdexcept>

rpcmanager::rpcmanager() { ; }

rpcmanager::~rpcmanager() { ; }

rpcserver_booter_base* rpcmanager::get()
{
    if (aggregator_registered) {
        return aggregator.get();
    } else if (booter_registered) {
        return boot.get();
    }
    assert(booter_registered || aggregator_registered);
    return boot.get();
}

void rpcmanager::register_booter(rpcserver_booter_base* booter)
{
    if (make_aggregator && !aggregator_registered) {
        aggregator = std::make_unique<rpcserver_booter_aggregator>();
        aggregator_registered = true;
    }

    if (aggregator_registered) {
        rpcmanager::rpcserver_booter_base_sptr bootreg(booter);
        aggregator->agg()->registerServer(bootreg);
    } else if (!booter_registered) {
        boot.reset(booter);
        booter_registered = true;
    } else {
        throw std::runtime_error("rpcmanager: Aggregator not in use, and a rpc booter is "
                                 "already registered");
    }
}
