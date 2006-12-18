/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_runtime.h>
#include <gr_runtime_impl.h>
#include <gr_local_sighandler.h>
#include <iostream>

static gr_runtime *s_runtime = 0;

gr_runtime_sptr 
gr_make_runtime(gr_hier_block2_sptr top_block)
{
    return gr_runtime_sptr(new gr_runtime(top_block));
}

gr_runtime::gr_runtime(gr_hier_block2_sptr top_block)
{
    d_impl = new gr_runtime_impl(top_block);
    s_runtime = this;
}
  
gr_runtime::~gr_runtime()
{
    s_runtime = 0; // we don't own this
    delete d_impl;
}

// HACK: This prevents using more than one gr_runtime instance
static void 
runtime_sigint_handler(int signum)
{

    if (s_runtime)
        s_runtime->stop();
}

void 
gr_runtime::start()
{
    gr_local_sighandler sigint(SIGINT, runtime_sigint_handler);

    d_impl->start();
}

void 
gr_runtime::stop()
{
    d_impl->stop();
}

void 
gr_runtime::wait()
{
    gr_local_sighandler sigint(SIGINT, runtime_sigint_handler);

    d_impl->wait();
}

void 
gr_runtime::run()
{
    gr_local_sighandler sigint(SIGINT, runtime_sigint_handler);

    d_impl->start();
    d_impl->wait();
}
