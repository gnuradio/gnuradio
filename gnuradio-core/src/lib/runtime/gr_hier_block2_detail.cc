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

#include <gr_hier_block2_detail.h>
#include <gr_simple_flowgraph.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>

gr_hier_block2_detail::gr_hier_block2_detail(gr_hier_block2 *owner) :
d_owner(owner)
{
}

gr_hier_block2_detail::~gr_hier_block2_detail()
{
    d_owner = 0; // Don't use delete, we didn't allocate
}

gr_basic_block_sptr
gr_hier_block2_detail::lookup_block(const std::string &name)
{
    gr_hier_component_miter_t p = d_components.find(name);
    if (p != d_components.end())
        return p->second;
    else
        return gr_basic_block_sptr();
}

void 
gr_hier_block2_detail::define_component(const std::string &name, gr_basic_block_sptr block)
{
    if (!block)
	throw std::invalid_argument("null block passed");

    if (name == "self")
        throw std::invalid_argument("name is reserved");

    // TODO: reject names with '.' inside
    
    if (!lookup_block(name))
        d_components[name] = block;
    else
        throw std::invalid_argument("name already in use");
}

void 
gr_hier_block2_detail::connect(const std::string &src_name, int src_port, 
                               const std::string &dst_name, int dst_port)
{
    gr_io_signature_sptr src_io_signature;
    gr_io_signature_sptr dst_io_signature;
    
    // Check against our *input_signature* if we're wiring from one of our external inputs
    if (src_name == "self") 
        src_io_signature = d_owner->input_signature();
    else {
        gr_basic_block_sptr src_block = lookup_block(src_name);
        if (!src_block)
            throw std::invalid_argument("undefined src name");
        src_io_signature = src_block->output_signature();
    }
    
    // Check against our *output_signature* if we're wiring to one of our external outputs
    if (dst_name == "self") 
    	dst_io_signature = d_owner->output_signature();
    else {
        gr_basic_block_sptr dst_block = lookup_block(dst_name);
        if (!dst_block)
            throw std::invalid_argument("undefined dst name");
        dst_io_signature = dst_block->input_signature();
    }
    
    // Check port numbers are valid
    check_valid_port(src_io_signature, src_port);
    check_valid_port(dst_io_signature, dst_port);

    // Check destination port not already in use
    check_dst_not_used(dst_name, dst_port);

    // Check endpoint types match
    check_type_match(src_io_signature, src_port, dst_io_signature, dst_port);

    d_edges.push_back(gr_make_edge(src_name, src_port, dst_name, dst_port));
}

void 
gr_hier_block2_detail::check_valid_port(gr_io_signature_sptr sig, int port)
{
    if (port < 0)
        throw std::invalid_argument("port number must not be negative");
	
    if (sig->max_streams() >= 0 && port >= sig->max_streams())
        throw std::invalid_argument("port number exceeds max streams");
}

void 
gr_hier_block2_detail::check_dst_not_used(const std::string name, int port)
{
    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
        if ((*p)->dst_name() == name && (*p)->dst_port() == port)
            throw std::invalid_argument("destination port in use");
}

void 
gr_hier_block2_detail::check_type_match(gr_io_signature_sptr src_sig, int src_port,
                                        gr_io_signature_sptr dst_sig, int dst_port)
{
    if (src_sig->sizeof_stream_item(src_port) != dst_sig->sizeof_stream_item(dst_port))
        throw std::invalid_argument("type mismatch");
}

std::string
gr_hier_block2_detail::prepend_prefix(const std::string &prefix, const std::string &str)
{
    return prefix + ((prefix == "") ? "" : ".") + str;
}

gr_endpoint
gr_hier_block2_detail::match_endpoint(const std::string &name, int port, bool is_input)
{
    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
        if (is_input) {
            if ((*p)->src_name() == name && (*p)->src_port() == port)
                return resolve_endpoint((*p)->dst_name(), (*p)->dst_port(), "", !is_input);
	    }
        else {
            if ((*p)->dst_name() == name && (*p)->dst_port() == port)
                return resolve_endpoint((*p)->src_name(), (*p)->src_port(), "", !is_input);
        }
    }

    // Should never get here
    throw std::runtime_error("unable to match endpoint");
}

gr_endpoint
gr_hier_block2_detail::resolve_endpoint(const std::string &name, int port, 
                                        const std::string &prefix, bool is_input)
{
    gr_basic_block_sptr basic_block = lookup_block(name);

    // Check if 'name' points to gr_block (leaf node)
    gr_block_sptr block(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(basic_block));
    if (block)
        return gr_endpoint(prepend_prefix(prefix, name), port);

    // Check if 'name' points to hierarchical block
    gr_hier_block2_sptr hier_block2(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(basic_block));
    if (hier_block2) {
        std::string child_prefix = prepend_prefix(prefix, name);
        gr_endpoint match(hier_block2->d_detail->match_endpoint("self", port, !is_input));
        return gr_endpoint(prepend_prefix(child_prefix, match.name()), match.port());
    }

    // Shouldn't ever get here
    throw std::runtime_error("unable to resolve endpoint");
}

void
gr_hier_block2_detail::flatten(gr_simple_flowgraph_sptr sfg, const std::string &prefix)
{
    flatten_components(sfg, prefix);
    flatten_edges(sfg, prefix);
}

void
gr_hier_block2_detail::flatten_components(gr_simple_flowgraph_sptr sfg, const std::string &prefix)
{
    // Add my non-hierarchical components to the simple flowgraph, then recurse
    for (gr_hier_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
        std::string name = prepend_prefix(prefix, p->first);

        gr_basic_block_sptr basic_block = p->second;
        gr_block_sptr block(boost::dynamic_pointer_cast<gr_block, gr_basic_block>(basic_block));
        if (block)	
            sfg->define_component(name, block);

        gr_hier_block2_sptr hier_block2(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(basic_block));
        if (hier_block2)
            hier_block2->d_detail->flatten_components(sfg, name);
    }
}

void
gr_hier_block2_detail::flatten_edges(gr_simple_flowgraph_sptr sfg, const std::string &prefix)
{
    // Add my edges to the flow graph, resolving references to actual endpoints
    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
    	// Connections to self get resolved/added by parent if actually connected
        if ((*p)->src_name() == "self" || (*p)->dst_name() == "self")
            continue;

        gr_endpoint src_endp = resolve_endpoint((*p)->src_name(), (*p)->src_port(), prefix, true);
        gr_endpoint dst_endp = resolve_endpoint((*p)->dst_name(), (*p)->dst_port(), prefix, false);
        sfg->connect(src_endp.name(), src_endp.port(), dst_endp.name(), dst_endp.port());
    }

    // Recurse hierarchical children
    for (gr_hier_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
        gr_hier_block2_sptr hier_block2(boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(p->second));
        if (hier_block2)
            hier_block2->d_detail->flatten_edges(sfg, prepend_prefix(prefix, p->first));
    }
}
