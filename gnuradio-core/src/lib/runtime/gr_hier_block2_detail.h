/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_HIER_BLOCK2_DETAIL_H
#define INCLUDED_GR_HIER_BLOCK2_DETAIL_H

#include <gr_core_api.h>
#include <gr_hier_block2.h>
#include <gr_flat_flowgraph.h>
#include <boost/utility.hpp>

/*!
 * \ingroup internal
 */
class GR_CORE_API gr_hier_block2_detail : boost::noncopyable
{
public:
  gr_hier_block2_detail(gr_hier_block2 *owner);
  ~gr_hier_block2_detail();

  void connect(gr_basic_block_sptr block);
  void connect(gr_basic_block_sptr src, int src_port, 
	       gr_basic_block_sptr dst, int dst_port);
  void disconnect(gr_basic_block_sptr block);
  void disconnect(gr_basic_block_sptr, int src_port, 
		  gr_basic_block_sptr, int dst_port);
  void disconnect_all();
  void lock();
  void unlock();
  void flatten_aux(gr_flat_flowgraph_sptr sfg) const;

private:
  
  // Private implementation data
  gr_hier_block2 *d_owner;
  gr_hier_block2_detail *d_parent_detail;
  gr_flowgraph_sptr d_fg;
  std::vector<gr_endpoint_vector_t> d_inputs; // Multiple internal endpoints per external input
  gr_endpoint_vector_t d_outputs;             // Single internal endpoint per external output
  gr_basic_block_vector_t d_blocks;
  
  void connect_input(int my_port, int port, gr_basic_block_sptr block);
  void connect_output(int my_port, int port, gr_basic_block_sptr block);
  void disconnect_input(int my_port, int port, gr_basic_block_sptr block);
  void disconnect_output(int my_port, int port, gr_basic_block_sptr block);

  gr_endpoint_vector_t resolve_port(int port, bool is_input);
  gr_endpoint_vector_t resolve_endpoint(const gr_endpoint &endp, bool is_input) const;
};

#endif /* INCLUDED_GR_HIER_BLOCK2_DETAIL_H */
