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

#ifndef INCLUDED_GR_FLOWGRAPH_H
#define INCLUDED_GR_FLOWGRAPH_H

#include <gr_core_api.h>
#include <gr_basic_block.h>
#include <iostream>

/*!
 * \brief Class representing a specific input or output graph endpoint
 * \ingroup internal
 */
class GR_CORE_API gr_endpoint
{
private:
  gr_basic_block_sptr d_basic_block;
  int d_port;

public:
  gr_endpoint() : d_basic_block(), d_port(0) { }
  gr_endpoint(gr_basic_block_sptr block, int port) { d_basic_block = block; d_port = port; }
  gr_basic_block_sptr block() const { return d_basic_block; }
  int port() const { return d_port; }

  bool operator==(const gr_endpoint &other) const;
};    

inline bool gr_endpoint::operator==(const gr_endpoint &other) const
{
  return (d_basic_block == other.d_basic_block && 
	  d_port == other.d_port);
}

// Hold vectors of gr_endpoint objects
typedef std::vector<gr_endpoint> gr_endpoint_vector_t;
typedef std::vector<gr_endpoint>::iterator gr_endpoint_viter_t;

/*!
 *\brief Class representing a connection between to graph endpoints
 *
 */
class GR_CORE_API gr_edge
{
public:
  gr_edge() : d_src(), d_dst() { };
  gr_edge(const gr_endpoint &src, const gr_endpoint &dst) : d_src(src), d_dst(dst) { }
  ~gr_edge();

  const gr_endpoint &src() const { return d_src; }
  const gr_endpoint &dst() const { return d_dst; }

private:
  gr_endpoint d_src;
  gr_endpoint d_dst;
};

// Hold vectors of gr_edge objects
typedef std::vector<gr_edge> gr_edge_vector_t;
typedef std::vector<gr_edge>::iterator gr_edge_viter_t;


// Create a shared pointer to a heap allocated flowgraph
// (types defined in gr_runtime_types.h)
GR_CORE_API gr_flowgraph_sptr gr_make_flowgraph();

/*!
 * \brief Class representing a directed, acyclic graph of basic blocks
 * \ingroup internal
 */
class GR_CORE_API gr_flowgraph
{
public:
  friend GR_CORE_API gr_flowgraph_sptr gr_make_flowgraph();

  // Destruct an arbitrary flowgraph
  ~gr_flowgraph();

  // Connect two endpoints
  void connect(const gr_endpoint &src, const gr_endpoint &dst);

  // Disconnect two endpoints
  void disconnect(const gr_endpoint &src, const gr_endpoint &dst);

  // Connect an output port to an input port (convenience)
  void connect(gr_basic_block_sptr src_block, int src_port,
	       gr_basic_block_sptr dst_block, int dst_port);

  // Disconnect an input port from an output port (convenience)
  void disconnect(gr_basic_block_sptr src_block, int src_port,
		  gr_basic_block_sptr dst_block, int dst_port);

  // Validate connectivity, raise exception if invalid
  void validate();

  // Clear existing flowgraph
  void clear();

  // Return vector of edges
  const gr_edge_vector_t &edges() const { return d_edges; }

  // Return vector of connected blocks
  gr_basic_block_vector_t calc_used_blocks();

  // Return toplogically sorted vector of blocks.  All the sources come first.
  gr_basic_block_vector_t topological_sort(gr_basic_block_vector_t &blocks);

  // Return vector of vectors of disjointly connected blocks, topologically
  // sorted.
  std::vector<gr_basic_block_vector_t> partition();

protected:
  gr_basic_block_vector_t d_blocks;
  gr_edge_vector_t d_edges;

  gr_flowgraph();
  std::vector<int> calc_used_ports(gr_basic_block_sptr block, bool check_inputs); 
  gr_basic_block_vector_t calc_downstream_blocks(gr_basic_block_sptr block, int port);
  gr_edge_vector_t calc_upstream_edges(gr_basic_block_sptr block);
  bool has_block_p(gr_basic_block_sptr block);
  gr_edge calc_upstream_edge(gr_basic_block_sptr block, int port);

private:

  void check_valid_port(gr_io_signature_sptr sig, int port);
  void check_dst_not_used(const gr_endpoint &dst);
  void check_type_match(const gr_endpoint &src, const gr_endpoint &dst);
  gr_edge_vector_t calc_connections(gr_basic_block_sptr block, bool check_inputs); // false=use outputs
  void check_contiguity(gr_basic_block_sptr block, const std::vector<int> &used_ports, bool check_inputs);

  gr_basic_block_vector_t calc_downstream_blocks(gr_basic_block_sptr block);
  gr_basic_block_vector_t calc_reachable_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  void reachable_dfs_visit(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  gr_basic_block_vector_t calc_adjacent_blocks(gr_basic_block_sptr block, gr_basic_block_vector_t &blocks);
  gr_basic_block_vector_t sort_sources_first(gr_basic_block_vector_t &blocks);
  bool source_p(gr_basic_block_sptr block);
  void topological_dfs_visit(gr_basic_block_sptr block, gr_basic_block_vector_t &output);
};

// Convenience functions
inline
void gr_flowgraph::connect(gr_basic_block_sptr src_block, int src_port,
		      gr_basic_block_sptr dst_block, int dst_port)
{
  connect(gr_endpoint(src_block, src_port),
	  gr_endpoint(dst_block, dst_port));
}

inline
void gr_flowgraph::disconnect(gr_basic_block_sptr src_block, int src_port,
			 gr_basic_block_sptr dst_block, int dst_port)
{
  disconnect(gr_endpoint(src_block, src_port),
	     gr_endpoint(dst_block, dst_port));
}

inline std::ostream&
operator <<(std::ostream &os, const gr_endpoint endp)
{
  os << endp.block() << ":" << endp.port();
  return os;
}

inline std::ostream&
operator <<(std::ostream &os, const gr_edge edge)
{
  os << edge.src() << "->" << edge.dst();
  return os;
}

#endif /* INCLUDED_GR_FLOWGRAPH_H */
