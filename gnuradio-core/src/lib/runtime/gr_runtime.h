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

#ifndef INCLUDED_GR_RUNTIME_H
#define INCLUDED_GR_RUNTIME_H

#include <gr_runtime_types.h>

class gr_runtime_impl;

gr_runtime_sptr gr_make_runtime(gr_hier_block2_sptr top_block);

class gr_runtime
{
private:
    gr_runtime(gr_hier_block2_sptr top_block);
    friend gr_runtime_sptr gr_make_runtime(gr_hier_block2_sptr top_block);

    gr_runtime_impl *d_impl;
    
public:
    ~gr_runtime();

    void start();
    void stop();
    void wait();
    void run();
};

#endif /* INCLUDED_GR_RUNTIME_H */
