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
#ifndef INCLUDED_GR_HIER_BLOCK2_DETAIL_H
#define INCLUDED_GR_HIER_BLOCK2_DETAIL_H

#include <gr_hier_block2.h>
#include <gr_simple_flowgraph_detail.h>
#include <boost/utility.hpp>

typedef std::map<std::string, gr_basic_block_sptr> gr_hier_component_map_t;
typedef std::map<std::string, gr_basic_block_sptr>::iterator gr_hier_component_miter_t;

class gr_hier_block2_detail : boost::noncopyable
{
private:
    friend class gr_hier_block2;
    friend class gr_runtime_impl;
    
    // Constructor--it's private, only friends can instantiate
    gr_hier_block2_detail(gr_hier_block2 *owner);

    // Private implementation data
    gr_hier_block2 *d_owner;
    gr_hier_component_map_t d_components;
    gr_edge_vector_t d_edges;
        
    // Private implementation methods
    void define_component(const std::string &name, gr_basic_block_sptr block);
    gr_basic_block_sptr lookup_block(const std::string &name);
    void connect(const std::string &src_name, int src_port, 
                 const std::string &dst_name, int dst_port);
    void check_valid_port(gr_io_signature_sptr sig, int port);
    void check_dst_not_used(const std::string name, int port);
    void check_type_match(gr_io_signature_sptr src_sig, int src_port,
                          gr_io_signature_sptr dst_sig, int dst_port);
    std::string prepend_prefix(const std::string &prefix, const std::string &str);
    void flatten(gr_simple_flowgraph_sptr sfg, const std::string &prefix = "");
    void flatten_components(gr_simple_flowgraph_sptr sfg, const std::string &prefix);
    void flatten_edges(gr_simple_flowgraph_sptr sfg, const std::string &prefix);
    gr_endpoint match_endpoint(const std::string &name, int port, bool is_input);
    gr_endpoint resolve_endpoint(const std::string &name, int port, 
                                 const std::string &prefix, bool is_input);
    
public:
    ~gr_hier_block2_detail();
};

#endif /* INCLUDED_GR_HIER_BLOCK2_DETAIL_H */
