/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_FLOWGRAPH_H
#define INCLUDED_GR_SIMPLE_FLOWGRAPH_H

#include <gr_block.h>
#include <boost/shared_ptr.hpp>
#include <string>

gr_simple_flowgraph_sptr gr_make_simple_flowgraph();

class gr_simple_flowgraph_detail;

class gr_endpoint
{
private:
  friend class gr_hier_block2_detail;
  gr_basic_block_sptr d_block;
  int d_port;

public:
  gr_endpoint() : d_block(), d_port(0) { } // Internal use only
  gr_endpoint(gr_basic_block_sptr block, int port) { d_block = block; d_port = port; }
  gr_basic_block_sptr block() const { return d_block; }
  int port() const { return d_port; }
};    

typedef std::vector<gr_endpoint> gr_endpoint_vector_t;

/*!
 *\brief Class representing a low-level, "flattened" flow graph
 *
 * This class holds the results of the call to gr.hier_block2.flatten(),
 * which is a graph that has only the connected blocks and edges of
 * the original hier_block2, with all the hierarchy removed.
 *
 * While this class is exported to Python via SWIG for ease of QA
 * testing, it is not used by application developers, and there is
 * no way to feed this to a gr.runtime() instance.
 */
class gr_simple_flowgraph
{
private:
  friend class gr_runtime_impl;
  friend class gr_simple_flowgraph_detail;
  friend class gr_hier_block2_detail;
  friend gr_simple_flowgraph_sptr gr_make_simple_flowgraph();
  gr_simple_flowgraph();

  gr_simple_flowgraph_detail *d_detail;
            
public:
  ~gr_simple_flowgraph();

  void connect(gr_basic_block_sptr src_block, int src_port,
	       gr_basic_block_sptr dst_block, int dst_port);
  void connect(const gr_endpoint &src, const gr_endpoint &dst);
  void disconnect(gr_basic_block_sptr src_block, int src_port,
		  gr_basic_block_sptr dst_block, int dst_port);
  void validate();
};

#endif /* INCLUDED_GR_SIMPLE_FLOWGRAPH_H */
