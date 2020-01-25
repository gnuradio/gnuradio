/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCSERVER_BOOTER_BASE
#define RPCSERVER_BOOTER_BASE

#include <string>
#include <vector>

class rpcserver_base;

class rpcserver_booter_base
{
public:
    rpcserver_booter_base() { ; }
    virtual ~rpcserver_booter_base() { ; }

    virtual rpcserver_base* i() = 0;
    virtual const std::vector<std::string> endpoints() = 0;
    virtual const std::string& type() = 0;

private:
};

#endif /* RPCSERVER_BOOTER_BASE */
