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

#include <gr_simple_flowgraph.h>
#include <gr_simple_flowgraph_detail.h>
#include <iostream>

#define GR_SIMPLE_FLOWGRAPH_DEBUG 0

gr_simple_flowgraph_sptr gr_make_simple_flowgraph()
{
    return gr_simple_flowgraph_sptr(new gr_simple_flowgraph());
}

gr_simple_flowgraph::gr_simple_flowgraph() :
d_detail(new gr_simple_flowgraph_detail())
{
}
  
gr_simple_flowgraph::~gr_simple_flowgraph()
{
    delete d_detail;
}

void
gr_simple_flowgraph::define_component(const std::string &name, gr_block_sptr block)
{
    if (GR_SIMPLE_FLOWGRAPH_DEBUG)
        std::cout << "Defining block " << block << " as " << name << std::endl;
    d_detail->define_component(name, block);
}

void
gr_simple_flowgraph::connect(const std::string &src_name, int src_port,
                             const std::string &dst_name, int dst_port)
{
    d_detail->connect(src_name, src_port, dst_name, dst_port);
}

void
gr_simple_flowgraph::validate()
{
    d_detail->validate();
}
