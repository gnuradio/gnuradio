/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_FLOWGRAPH_H
#define INCLUDED_GR_RUNTIME_FLOWGRAPH_H

#include <gnuradio/api.h>
#include <gnuradio/basic_block.h>
#include <gnuradio/io_signature.h>
#include <iostream>

namespace gr {

  /*!
   * \brief Class representing a specific input or output graph endpoint
   * \ingroup internal
   */
  class GR_RUNTIME_API endpoint
  {
  private:
    basic_block_sptr d_basic_block;
    int d_port;

  public:
    endpoint() : d_basic_block(), d_port(0) { }
    endpoint(basic_block_sptr block, int port) { d_basic_block = block; d_port = port; }
    basic_block_sptr block() const { return d_basic_block; }
    int port() const { return d_port; }

    bool operator==(const endpoint &other) const;
  };

  inline bool endpoint::operator==(const endpoint &other) const
  {
    return (d_basic_block == other.d_basic_block &&
            d_port == other.d_port);
  }

  class GR_RUNTIME_API msg_endpoint
  {
  private:
    basic_block_sptr d_basic_block;
    pmt::pmt_t d_port;
    bool d_is_hier;

  public:
    msg_endpoint() : d_basic_block(), d_port(pmt::PMT_NIL) { }
    msg_endpoint(basic_block_sptr block, pmt::pmt_t port, bool is_hier=false) {
      d_basic_block = block; d_port = port; d_is_hier = is_hier;
    }
    basic_block_sptr block() const { return d_basic_block; }
    pmt::pmt_t port() const { return d_port; }
    bool is_hier() const { return d_is_hier; }
    void set_hier(bool h) { d_is_hier = h; }

    bool operator==(const msg_endpoint &other) const;
  };

  inline bool
  msg_endpoint::operator==(const msg_endpoint &other) const
  {
    return (d_basic_block == other.d_basic_block &&
            pmt::equal(d_port, other.d_port));
  }

  // Hold vectors of gr::endpoint objects
  typedef std::vector<endpoint> endpoint_vector_t;
  typedef std::vector<endpoint>::iterator endpoint_viter_t;

  /*!
   *\brief Class representing a connection between to graph endpoints
   */
  class GR_RUNTIME_API edge
  {
  public:
    edge() : d_src(), d_dst() { };
    edge(const endpoint &src, const endpoint &dst)
      : d_src(src), d_dst(dst) { }
    ~edge();

    const endpoint &src() const { return d_src; }
    const endpoint &dst() const { return d_dst; }

  private:
    endpoint d_src;
    endpoint d_dst;
  };

  // Hold vectors of gr::edge objects
  typedef std::vector<edge> edge_vector_t;
  typedef std::vector<edge>::iterator edge_viter_t;


  /*!
   *\brief Class representing a msg connection between to graph msg endpoints
   */
  class GR_RUNTIME_API msg_edge
  {
  public:
    msg_edge() : d_src(), d_dst() { };
    msg_edge(const msg_endpoint &src, const msg_endpoint &dst)
      : d_src(src), d_dst(dst) { }
    ~msg_edge() {}

    const msg_endpoint &src() const { return d_src; }
    const msg_endpoint &dst() const { return d_dst; }

  private:
    msg_endpoint d_src;
    msg_endpoint d_dst;
  };

  // Hold vectors of gr::msg_edge objects
  typedef std::vector<msg_edge> msg_edge_vector_t;
  typedef std::vector<msg_edge>::iterator msg_edge_viter_t;

  // Create a shared pointer to a heap allocated flowgraph
  // (types defined in runtime_types.h)
  GR_RUNTIME_API flowgraph_sptr make_flowgraph();

  /*!
   * \brief Class representing a directed, acyclic graph of basic blocks
   * \ingroup internal
   */
  class GR_RUNTIME_API flowgraph
  {
  public:
    friend GR_RUNTIME_API flowgraph_sptr make_flowgraph();

    // Destruct an arbitrary flowgraph
    ~flowgraph();

    // Connect two endpoints
    void connect(const endpoint &src, const endpoint &dst);

    // Disconnect two endpoints
    void disconnect(const endpoint &src, const endpoint &dst);

    // Connect an output port to an input port (convenience)
    void connect(basic_block_sptr src_block, int src_port,
                 basic_block_sptr dst_block, int dst_port);

    // Disconnect an input port from an output port (convenience)
    void disconnect(basic_block_sptr src_block, int src_port,
                    basic_block_sptr dst_block, int dst_port);

    // Connect two msg endpoints
    void connect(const msg_endpoint &src, const msg_endpoint &dst);
  
    // Disconnect two msg endpoints
    void disconnect(const msg_endpoint &src, const msg_endpoint &dst);

    // Validate connectivity, raise exception if invalid
    void validate();

    // Clear existing flowgraph
    void clear();

    // Return vector of edges
    const edge_vector_t &edges() const { return d_edges; }
  
    // Return vector of msg edges
    const msg_edge_vector_t &msg_edges() const { return d_msg_edges; }

    // Return vector of connected blocks
    basic_block_vector_t calc_used_blocks();

    // Return toplogically sorted vector of blocks.  All the sources come first.
    basic_block_vector_t topological_sort(basic_block_vector_t &blocks);

    // Return vector of vectors of disjointly connected blocks,
    // topologically sorted.
    std::vector<basic_block_vector_t> partition();

  protected:
    basic_block_vector_t d_blocks;
    edge_vector_t d_edges;
    msg_edge_vector_t d_msg_edges;

    flowgraph();
    std::vector<int> calc_used_ports(basic_block_sptr block, bool check_inputs);
    basic_block_vector_t calc_downstream_blocks(basic_block_sptr block, int port);
    edge_vector_t calc_upstream_edges(basic_block_sptr block);
    bool has_block_p(basic_block_sptr block);
    edge calc_upstream_edge(basic_block_sptr block, int port);

  private:
    void check_valid_port(gr::io_signature::sptr sig, int port);
    void check_valid_port(const msg_endpoint &e);
    void check_dst_not_used(const endpoint &dst);
    void check_type_match(const endpoint &src, const endpoint &dst);
    edge_vector_t calc_connections(basic_block_sptr block, bool check_inputs); // false=use outputs
    void check_contiguity(basic_block_sptr block, const std::vector<int> &used_ports, bool check_inputs);

    basic_block_vector_t calc_downstream_blocks(basic_block_sptr block);
    basic_block_vector_t calc_reachable_blocks(basic_block_sptr block, basic_block_vector_t &blocks);
    void reachable_dfs_visit(basic_block_sptr block, basic_block_vector_t &blocks);
    basic_block_vector_t calc_adjacent_blocks(basic_block_sptr block, basic_block_vector_t &blocks);
    basic_block_vector_t sort_sources_first(basic_block_vector_t &blocks);
    bool source_p(basic_block_sptr block);
    void topological_dfs_visit(basic_block_sptr block, basic_block_vector_t &output);
  };

  // Convenience functions
  inline
  void flowgraph::connect(basic_block_sptr src_block, int src_port,
                          basic_block_sptr dst_block, int dst_port)
  {
    connect(endpoint(src_block, src_port),
            endpoint(dst_block, dst_port));
  }

  inline
  void flowgraph::disconnect(basic_block_sptr src_block, int src_port,
                             basic_block_sptr dst_block, int dst_port)
  {
    disconnect(endpoint(src_block, src_port),
               endpoint(dst_block, dst_port));
  }

  inline std::ostream&
  operator <<(std::ostream &os, const endpoint endp)
  {
    os << endp.block()->alias() << ":" << endp.port();
    return os;
  }

  inline std::ostream&
  operator <<(std::ostream &os, const edge edge)
  {
    os << edge.src() << "->" << edge.dst();
    return os;
  }

  inline std::ostream&
  operator <<(std::ostream &os, const msg_endpoint endp)
  {
    os << endp.block()->alias() << ":" << pmt::symbol_to_string(endp.port());
    return os;
  }

  inline std::ostream&
  operator <<(std::ostream &os, const msg_edge edge)
  {
    os << edge.src() << "->" << edge.dst();
    return os;
  }

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_FLOWGRAPH_H */
