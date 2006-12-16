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

#ifndef INCLUDED_GR_SIMPLE_FLOWGRAPH_DETAIL_H
#define INCLUDED_GR_SIMPLE_FLOWGRAPH_DETAIL_H

#include <gr_block.h>
#include <map>

#define GR_FIXED_BUFFER_SIZE (32*(1L<<10))

typedef std::map<std::string, gr_block_sptr> gr_component_map_t;
typedef std::map<std::string, gr_block_sptr>::iterator gr_component_miter_t;

class gr_endpoint
{
private:
    std::string d_name;
    int d_port;

public:
    gr_endpoint(const std::string &name, int port) { d_name = name; d_port = port; }
    const std::string &name() const { return d_name; }
    int port() const { return d_port; }
};    

class gr_edge;
typedef boost::shared_ptr<gr_edge> gr_edge_sptr;
gr_edge_sptr gr_make_edge(const std::string &src_name, int src_port, 
                          const std::string &dst_name, int dst_port);

class gr_edge
{
private:
    friend gr_edge_sptr gr_make_edge(const std::string &src_name, int src_port,
                                     const std::string &dst_name, int dst_port);
    gr_edge(const std::string &name, int src_port,
            const std::string &name, int dst_port);

    gr_endpoint d_src;
    gr_endpoint d_dst;

public:
    ~gr_edge();
    const std::string src_name() const { return d_src.name(); }
    const std::string dst_name() const { return d_dst.name(); }
    int src_port() const { return d_src.port(); }
    int dst_port() const { return d_dst.port(); }
};

typedef std::vector<gr_edge_sptr> gr_edge_vector_t;
typedef std::vector<gr_edge_sptr>::iterator gr_edge_viter_t;

class gr_simple_flowgraph_detail
{
private:
    friend class gr_simple_flowgraph;
    friend class gr_runtime_impl;
    friend class topo_block_cmp;
    
    gr_simple_flowgraph_detail();

    gr_component_map_t d_components;
    gr_edge_vector_t   d_edges;
    static const unsigned int s_fixed_buffer_size = GR_FIXED_BUFFER_SIZE;
    
    void reset();
    void define_component(const std::string &name, gr_block_sptr block);    
    void connect(const std::string &src, int src_port, 
                 const std::string &dst, int dst_port);
    gr_block_sptr lookup_block(const std::string &name);
    std::string lookup_name(const gr_block_sptr block);

    void check_valid_port(gr_io_signature_sptr sig, int port);
    void check_dst_not_used(const std::string &name, int port);
    void check_type_match(gr_block_sptr src_block, int src_port,
                          gr_block_sptr dst_block, int dst_port);
    void validate();
    gr_edge_vector_t calc_connections(const std::string &name, bool check_inputs); // false=use outputs
    std::vector<int> calc_used_ports(const std::string &name, bool check_inputs); 
    void check_contiguity(gr_block_sptr block, const std::vector<int> &used_ports, 
                          bool check_inputs);
    void setup_connections();
    gr_buffer_sptr allocate_buffer(const std::string &name, int port);
    gr_block_vector_t calc_downstream_blocks(const std::string &name, int port);
    gr_block_vector_t calc_downstream_blocks(const std::string &name);
    gr_edge_vector_t calc_upstream_edges(const std::string &name);
    gr_block_vector_t calc_used_blocks();
    std::vector<gr_block_vector_t> partition();
    gr_block_vector_t calc_reachable_blocks(gr_block_sptr block, gr_block_vector_t &blocks);
    gr_block_vector_t topological_sort(gr_block_vector_t &blocks);
    void reachable_dfs_visit(gr_block_sptr block, gr_block_vector_t &blocks);
    gr_block_vector_t calc_adjacent_blocks(gr_block_sptr block, gr_block_vector_t &blocks);
    bool source_p(gr_block_sptr block);
    gr_block_vector_t sort_sources_first(gr_block_vector_t &blocks);
    void topological_dfs_visit(gr_block_sptr block, gr_block_vector_t &output);
    void dump_block_vector(gr_block_vector_t blocks);
        
public:
    ~gr_simple_flowgraph_detail();
};

inline std::ostream&
operator <<(std::ostream& os, const gr_block_sptr p)
{
    os << "<gr_block " << p->name() << " (" << p->unique_id() << ")>";
    return os;
}

inline std::ostream&
operator <<(std::ostream &os, const gr_endpoint endp)
{
    os << endp.name() << ":" << endp.port();
    return os;
}

#endif /* INCLUDED_GR_SIMPLE_FLOWGRAPH_H */
