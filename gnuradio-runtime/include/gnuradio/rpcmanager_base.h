/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCMANAGER_BASE_H
#define RPCMANAGER_BASE_H

#include <memory>

class rpcserver_booter_base;
// class rpcserver_booter_aggregator;

class rpcmanager_base
{
public:
    typedef std::shared_ptr<rpcserver_booter_base> rpcserver_booter_base_sptr;

    rpcmanager_base() { ; }
    ~rpcmanager_base() { ; }

    // static rpcserver_booter_base* get();

    // static void register_booter(rpcserver_booter_base_sptr booter);

private:
};

#endif /* RPCMANAGER_BASE_H */
