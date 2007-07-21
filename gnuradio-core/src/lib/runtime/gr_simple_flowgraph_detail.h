/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_FLOWGRAPH_DETAIL_H
#define INCLUDED_GR_SIMPLE_FLOWGRAPH_DETAIL_H

#include <gr_basic_block.h>
#include <gr_simple_flowgraph.h>
#include <iostream>

#define GR_FIXED_BUFFER_SIZE (32*(1L<<10))

class gr_edge;
typedef boost::shared_ptr<gr_edge> gr_edge_sptr;
gr_edge_sptr gr_make_edge(const gr_endpoint &src, const gr_endpoint &dst);

class gr_edge
{
private:
  friend gr_edge_sptr gr_make_edge(const gr_endpoint &src, const gr_endpoint &dst);
  gr_edge(const gr_endpoint &src, const gr_endpoint &dst) : d_src(src), d_dst(dst) { }

  gr_endpoint d_src;
  gr_endpoint d_dst;

public:
  ~gr_edge();

  const gr_endpoint &src() const { return d_src; }
  const gr_endpoint &dst() const { return d_dst; }
};

typedef std::vector<gr_edge_sptr> gr_edge_vector_t;
typedef std::vector<gr_edge_sptr>::iterator gr_edge_viter_t;

class gr_simple_flowgraph_detail
{
private:
  friend class gr_simple_flowgraph;
  friend class gr_runtime_impl;
  friend class gr_hier_block2_detail;
  friend class topo_block_cmp;
    
  gr_simple_flowgraph_detail() : d_blocks(), d_edges() { }

  gr_basic_block_vector_t d_blocks;
  gr_edge_vector_t  d_edges;
  static const unsigned int s_fixed_buffer_size = GR_FIXED_BUFFER_SIZE;
    
  void reset();
  void connect(const gr_endpoint &src, const gr_endpoint &dst);
  void disconnect(const gr_endpoint &src, const gr_endpoint &dst);
  void check_valid_port(gr_io_signature_sptr sig, int port);
  void check_dst_not_used(const gr_endpoint &dst);
  void check_type_match(const gr_endpoint &src, const gr_endpoint &dst);
  void validate();
  gr_edge_vector_t calc_connections(gr_basic_block_sptr block, bool check_inputs); // false=use outputs
  std::vector<int> calc_used_ports(gr_basic_block_sptr block, bool check_inputs); 
  void check_contiguity(gr_basic_block_sptr block, const std::vector<int> &used_ports, bool check_inputs);
  void setup_connections();
  void merge_connections(gr_simple_flowgraph_sptr sfg);

  void connect_block_inputs(gr_basic_block_sptr block);
  gr_block_detail_sptr allocate_block_detail(gr_basic_block_sptr block, 
					     gr_block_detail_sptr old_detail=gr_block_detail_sptr());
  gr_buffer_sptr allocate_buffer(gr_basic_block_sptr block, int port);
  gr_basic_block_vector_t calc_downstream_blocks(gr_basic_block_sptr block, int port);
  gr_basic_block_vector_t calc_downstream_blocks(gr_basic_block_sptr block);
  gr_edge_sptr calc_upstream_edge(gr_basic_block_sptr block, int port);
  gr_edge_vector_t calc_upstream_edges(gr_basic_block_sptr block);
  gr_basic_block_vector_t calc_used_blocks();
  std::vector<gr_block_vector_t> partition();
  gr_basic_block_vector_t calc_reachable_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  gr_block_vector_t topological_sort(gr_basic_block_vector_t &blocks);
  void reachable_dfs_visit(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  gr_basic_block_vector_t calc_adjacent_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  bool source_p(gr_basic_block_sptr block);
  gr_basic_block_vector_t sort_sources_first(gr_basic_block_vector_t &blocks);
  void topological_dfs_visit(gr_basic_block_sptr block, gr_block_vector_t &output);
  bool has_block_p(gr_basic_block_sptr block);

public:
  ~gr_simple_flowgraph_detail();
};

inline std::ostream&
operator <<(std::ostream &os, const gr_endpoint endp)
{
  os << endp.block() << ":" << endp.port();
  return os;
}

inline std::ostream&
operator <<(std::ostream &os, const gr_edge_sptr edge)
{
  os << edge->src() << "->" << edge->dst();
  return os;
}

inline void
enumerate_edges(gr_edge_vector_t &edges)
{
  std::cout << "Edge list has " << edges.size() << " elements" << std::endl;
  for(gr_edge_viter_t p = edges.begin(); p != edges.end(); p++)
    std::cout << *p << std::endl;
}

#endif /* INCLUDED_GR_SIMPLE_FLOWGRAPH_H */
