/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "scheduler.h"

namespace gr {

scheduler::scheduler(flat_flowgraph_sptr ffg,
                     int max_noutput_items,
                     bool catch_exceptions)
{
}

scheduler::~scheduler() {}

void scheduler::register_error_handler(std::shared_ptr<basic_error_handler> handler)
{
    errorHandler = handler;
}

std::shared_ptr<basic_error_handler> scheduler::errorHandler;
} /* namespace gr */
