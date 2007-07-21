/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_simple_flowgraph.h>
#include <gr_simple_flowgraph_detail.h>
#include <iostream>

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
gr_simple_flowgraph::connect(gr_basic_block_sptr src_block, int src_port,
			     gr_basic_block_sptr dst_block, int dst_port)
{
  d_detail->connect(gr_endpoint(src_block, src_port), gr_endpoint(dst_block, dst_port));
}

void
gr_simple_flowgraph::connect(const gr_endpoint &src, const gr_endpoint &dst)
{
  d_detail->connect(src, dst);
}

void
gr_simple_flowgraph::disconnect(gr_basic_block_sptr src_block, int src_port,
				gr_basic_block_sptr dst_block, int dst_port)
{
  d_detail->disconnect(gr_endpoint(src_block, src_port), gr_endpoint(dst_block, dst_port));
}

void
gr_simple_flowgraph::validate()
{
  d_detail->validate();
}
