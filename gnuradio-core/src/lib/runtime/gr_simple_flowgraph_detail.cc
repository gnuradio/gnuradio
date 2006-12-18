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
#include <gr_io_signature.h>
#include <gr_block_detail.h>
#include <gr_buffer.h>
#include <iostream>
#include <stdexcept>

#define GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG 0

gr_edge_sptr
gr_make_edge(const std::string &src_name, int src_port,
	     const std::string &dst_name, int dst_port)
{
    return gr_edge_sptr(new gr_edge(src_name, src_port, dst_name, dst_port));
}

gr_edge::gr_edge(const std::string &src_name, int src_port, const std::string &dst_name, int dst_port)
  : d_src(src_name, src_port),
    d_dst(dst_name, dst_port)
{
}

gr_edge::~gr_edge()
{
}

gr_simple_flowgraph_detail::gr_simple_flowgraph_detail() :
d_components(),
d_edges()
{
}

gr_simple_flowgraph_detail::~gr_simple_flowgraph_detail()
{
}

void
gr_simple_flowgraph_detail::reset()
{
    // Boost shared pointers will deallocate as needed
    d_edges.clear();
    d_components.clear();
}

gr_block_sptr
gr_simple_flowgraph_detail::lookup_block(const std::string &name)
{
    gr_component_miter_t p = d_components.find(name);
    if (p != d_components.end())
        return p->second;
    else
        return gr_block_sptr();
}

std::string
gr_simple_flowgraph_detail::lookup_name(gr_block_sptr block)
{
    for (gr_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
        if (p->second == block)
            return p->first;
    }

    return std::string(""); // Not found
}

void
gr_simple_flowgraph_detail::define_component(const std::string &name, gr_block_sptr block)
{
    if (!block)
        throw std::invalid_argument("null block passed");

    if (!lookup_block(name))
        d_components[name] = block;
    else
        throw std::invalid_argument("name already in use");
}

void
gr_simple_flowgraph_detail::connect(const std::string &src_name, int src_port,
                                    const std::string &dst_name, int dst_port)
{
    gr_block_sptr src_block = lookup_block(src_name);
    gr_block_sptr dst_block = lookup_block(dst_name);

    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "Connecting " << src_name << ":" << src_port << "->"
                  << dst_name << ":" << dst_port << std::endl;

    if (!src_block)
        throw std::invalid_argument("unknown src name");
    if (!dst_block)
        throw std::invalid_argument("unknown dst name");

    check_valid_port(src_block->output_signature(), src_port);
    check_valid_port(dst_block->input_signature(), dst_port);
    check_dst_not_used(dst_name, dst_port);
    check_type_match(src_block, src_port, dst_block, dst_port);

    d_edges.push_back(gr_make_edge(src_name, src_port, dst_name, dst_port));
}

void
gr_simple_flowgraph_detail::check_valid_port(gr_io_signature_sptr sig, int port)
{
    if (port < 0)
        throw std::invalid_argument("negative port number");
    if (sig->max_streams() >= 0 && port >= sig->max_streams())
        throw std::invalid_argument("port number exceeds max");
}

void
gr_simple_flowgraph_detail::check_dst_not_used(const std::string &name, int port)
{
    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
	if ((*p)->dst_name() == name && (*p)->dst_port() == port)
	    throw std::invalid_argument("dst already in use");
}

void
gr_simple_flowgraph_detail::check_type_match(gr_block_sptr src_block, int src_port,
                                             gr_block_sptr dst_block, int dst_port)
{
    int src_size = src_block->output_signature()->sizeof_stream_item(src_port);
    int dst_size = dst_block->input_signature()->sizeof_stream_item(dst_port);

    if (src_size != dst_size)
        throw std::invalid_argument("type size mismatch");
}

void
gr_simple_flowgraph_detail::validate()
{
    for (gr_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
    	std::vector<int> used_ports;
    	int ninputs, noutputs;

        used_ports = calc_used_ports(p->first, true); // inputs
    	ninputs = used_ports.size();
    	check_contiguity(p->second, used_ports, true); // inputs

    	used_ports = calc_used_ports(p->first, false); // outputs
    	noutputs = used_ports.size();
    	check_contiguity(p->second, used_ports, false); // outputs

    	if (!(p->second->check_topology(ninputs, noutputs)))
    	    throw std::runtime_error("check topology failed");
    }
}

std::vector<int>
gr_simple_flowgraph_detail::calc_used_ports(const std::string &name, bool check_inputs)
{
    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "Calculating used " << (check_inputs ? "input " : "output ")
                  << "ports...";

    std::vector<int> tmp, result;
    std::insert_iterator<std::vector<int> > inserter(result, result.begin());

    gr_edge_vector_t edges = calc_connections(name, check_inputs);

    for (gr_edge_viter_t p = edges.begin(); p != edges.end(); p++) {
        if (check_inputs == true)
            tmp.push_back((*p)->dst_port());
        else
            tmp.push_back((*p)->src_port());
    }

    // remove duplicates
    std::sort(tmp.begin(), tmp.end());
    std::unique_copy(tmp.begin(), tmp.end(), inserter);

    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << result.size() << std::endl;

    return result;
}

gr_edge_vector_t
gr_simple_flowgraph_detail::calc_connections(const std::string &name, bool check_inputs)
{
    gr_edge_vector_t result;

    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
        if (check_inputs) {
            if ((*p)->dst_name() == name)
                result.push_back(*p);
        }
        else {
            if ((*p)->src_name() == name)
                result.push_back(*p);
        }
    }

    return result;    // assumes no duplicates
}

void
gr_simple_flowgraph_detail::check_contiguity(gr_block_sptr block,
                                             const std::vector<int> &used_ports,
                                             bool check_inputs)
{
    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "Checking " << (check_inputs ? "input " : "output ")
                  << "contiguity...";

    gr_io_signature_sptr sig =
        check_inputs ? block->input_signature() : block->output_signature();

    int nports = used_ports.size();
    int min_ports = sig->min_streams();

    if (nports == 0) {
        if (min_ports == 0) {
            if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
                std::cout << "ok." << std::endl;
            return;
        }
        else {
            if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
                std::cout << "needs " << min_ports << ", only has "
                          << nports << std::endl;

            throw std::runtime_error("insufficient ports");
        }
    }

    if (used_ports[nports-1]+1 != nports) {
        for (int i = 0; i < nports; i++) {
            if (used_ports[i] != i) {
                if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
	                std::cout << "missing " << (check_inputs ? "input ":"output ")
                              << i << std::endl;

                throw std::runtime_error("missing input assignment");
	        }
	    }
    }

    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "ok." << std::endl;
}

void
gr_simple_flowgraph_detail::setup_connections()
{
    // Assign block details to component blocks
    for (gr_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
        if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
            std::cout << "Allocating output buffers for " << p->first << "..." << std::endl;

        int ninputs = calc_used_ports(p->first, true).size();
        int noutputs = calc_used_ports(p->first, false).size();
        gr_block_detail_sptr detail = gr_make_block_detail(ninputs, noutputs);
        for (int i = 0; i < noutputs; i++)
            detail->set_output(i, allocate_buffer(p->first, i));
        p->second->set_detail(detail);
    }

    // Connect inputs to outputs for each block
    for(gr_component_miter_t p = d_components.begin(); p != d_components.end(); p++) {
        // Get its detail and edges that feed into it
        gr_block_detail_sptr detail = p->second->detail();
        gr_edge_vector_t in_edges = calc_upstream_edges(p->first);

        if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
	        if (in_edges.size() > 0)
        	    std::cout << "Connecting inputs to " << p->first << "..." << std::endl;

        // For each edge that feeds into it
        for (gr_edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++) {
            // Set the input buffer on the destination port to the output
            // buffer on the source port
            int dst_port = (*e)->dst_port();
            int src_port = (*e)->src_port();
            gr_block_sptr src_block = lookup_block((*e)->src_name());
            gr_buffer_sptr src_buffer = src_block->detail()->output(src_port);

            if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
                std::cout << "Setting input on " << (*e)->dst_name()
                          << ":" << dst_port << std::endl;

            detail->set_input(dst_port, gr_buffer_add_reader(src_buffer, p->second->history()-1));
        }
    }
}

gr_buffer_sptr
gr_simple_flowgraph_detail::allocate_buffer(const std::string &name, int port)
{
    gr_block_sptr block = lookup_block(name);
    int item_size = block->output_signature()->sizeof_stream_item(port);
    int nitems = s_fixed_buffer_size/item_size;

    // Make sure there are at least twice the output_multiple no. of items
    if (nitems < 2*block->output_multiple())	// Note: this means output_multiple()
        nitems = 2*block->output_multiple();	// can't be changed by block dynamically

    // If any downstream blocks are decimators and/or have a large output_multiple,
    // ensure we have a buffer at least twice their decimation factor*output_multiple
    gr_block_vector_t blocks = calc_downstream_blocks(name, port);
    for (gr_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        int decimation = (int)(1.0/(*p)->relative_rate());
        int multiple   = (*p)->output_multiple();
        int history    = (*p)->history();
        nitems = std::max(nitems, 2*(decimation*multiple+history));
    }

    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "Allocating buffer for port " << port << " with "
                  << nitems << " items of size " << item_size << std::endl;

    return gr_make_buffer(nitems, item_size);
}

gr_block_vector_t
gr_simple_flowgraph_detail::calc_downstream_blocks(const std::string &name, int port)
{
    gr_block_vector_t tmp, result;
    std::insert_iterator<gr_block_vector_t> inserter(result, result.begin());

    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
        if ((*p)->src_name() == name && (*p)->src_port() == port)
            tmp.push_back(lookup_block((*p)->dst_name()));

    // Remove duplicates
    sort(tmp.begin(), tmp.end());
    unique_copy(tmp.begin(), tmp.end(), inserter);
    return result;
}

gr_block_vector_t
gr_simple_flowgraph_detail::calc_downstream_blocks(const std::string &name)
{
    gr_block_vector_t tmp, result;
    std::insert_iterator<gr_block_vector_t> inserter(result, result.begin());

    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
        if ((*p)->src_name() == name)
            tmp.push_back(lookup_block((*p)->dst_name()));

    // Remove duplicates
    sort(tmp.begin(), tmp.end());
    unique_copy(tmp.begin(), tmp.end(), inserter);
    return result;
}

gr_edge_vector_t
gr_simple_flowgraph_detail::calc_upstream_edges(const std::string &name)
{
    gr_edge_vector_t result;

    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++)
        if ((*p)->dst_name() == name)
            result.push_back(*p);

    return result; // Assume no duplicates
}

gr_block_vector_t
gr_simple_flowgraph_detail::calc_used_blocks()
{
    std::vector<std::string> tmp, tmp_unique;
    std::insert_iterator<std::vector<std::string> > inserter(tmp_unique, tmp_unique.begin());
    gr_block_vector_t result;

    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
        tmp.push_back((*p)->src_name());
        tmp.push_back((*p)->dst_name());
    }

    sort(tmp.begin(), tmp.end());
    unique_copy(tmp.begin(), tmp.end(), inserter);

    for (std::vector<std::string>::iterator p = tmp_unique.begin();
         p != tmp_unique.end(); p++)
        result.push_back(lookup_block(*p));

    if (GR_SIMPLE_FLOWGRAPH_DETAIL_DEBUG)
        std::cout << "Flowgraph uses " << result.size()
                  << " distinct blocks." << std::endl;

    return result;
}

std::vector<gr_block_vector_t>
gr_simple_flowgraph_detail::partition()
{
    std::vector<gr_block_vector_t> result;
    gr_block_vector_t blocks = calc_used_blocks();
    gr_block_vector_t graph;

    while (blocks.size() > 0) {
        graph = calc_reachable_blocks(blocks[0], blocks);
        assert(graph.size());
        result.push_back(topological_sort(graph));

        for (gr_block_viter_t p = graph.begin(); p != graph.end(); p++)
            blocks.erase(find(blocks.begin(), blocks.end(), *p));
    }

    return result;
}

gr_block_vector_t
gr_simple_flowgraph_detail::calc_reachable_blocks(gr_block_sptr block, gr_block_vector_t &blocks)
{
    gr_block_vector_t result;

    // Mark all blocks as unvisited
    for (gr_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
        (*p)->detail()->set_color(gr_block_detail::WHITE);

    // Recursively mark all reachable blocks
    reachable_dfs_visit(block, blocks);

    // Collect all the blocks that have been visited
    for (gr_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
        if ((*p)->detail()->color() == gr_block_detail::BLACK)
            result.push_back(*p);

    return result;
}

// Recursively mark all reachable blocks from given block and block list
void 
gr_simple_flowgraph_detail::reachable_dfs_visit(gr_block_sptr block, gr_block_vector_t &blocks)
{
    // Mark the current one as visited
    block->detail()->set_color(gr_block_detail::BLACK);

    // Recurse into adjacent vertices
    gr_block_vector_t adjacent = calc_adjacent_blocks(block, blocks);

    for (gr_block_viter_t p = adjacent.begin(); p != adjacent.end(); p++)
        if ((*p)->detail()->color() == gr_block_detail::WHITE)
	        reachable_dfs_visit(*p, blocks);
}

// Return a list of block adjacent to a given block along any edge
gr_block_vector_t 
gr_simple_flowgraph_detail::calc_adjacent_blocks(gr_block_sptr block, gr_block_vector_t &blocks)
{
    gr_block_vector_t tmp, result;
    std::insert_iterator<gr_block_vector_t> inserter(result, result.begin());
    
    // Find any blocks that are inputs or outputs
    for (gr_edge_viter_t p = d_edges.begin(); p != d_edges.end(); p++) {
        if (lookup_block((*p)->src_name()) == block)
            tmp.push_back(lookup_block((*p)->dst_name()));
        if (lookup_block((*p)->dst_name()) == block)
            tmp.push_back(lookup_block((*p)->src_name()));
    }    

    // Remove duplicates
    sort(tmp.begin(), tmp.end());
    unique_copy(tmp.begin(), tmp.end(), inserter);
    return result;
}

gr_block_vector_t
gr_simple_flowgraph_detail::topological_sort(gr_block_vector_t &blocks)
{
    gr_block_vector_t result, tmp;
    tmp = sort_sources_first(blocks);

    // Start 'em all white
    for (gr_block_viter_t p = tmp.begin(); p != tmp.end(); p++)
        (*p)->detail()->set_color(gr_block_detail::WHITE);

    for (gr_block_viter_t p = tmp.begin(); p != tmp.end(); p++) {
        if ((*p)->detail()->color() == gr_block_detail::WHITE)
            topological_dfs_visit(*p, result);
    }    

    reverse(result.begin(), result.end());

    return result;
}

bool
gr_simple_flowgraph_detail::source_p(gr_block_sptr block)
{
    return (calc_upstream_edges(lookup_name(block)).size() == 0);
}

gr_block_vector_t
gr_simple_flowgraph_detail::sort_sources_first(gr_block_vector_t &blocks)
{
    gr_block_vector_t sources, nonsources, result;

    for (gr_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        if (source_p(*p))
            sources.push_back(*p);
        else
            nonsources.push_back(*p);
    }

    for (gr_block_viter_t p = sources.begin(); p != sources.end(); p++)
        result.push_back(*p);

    for (gr_block_viter_t p = nonsources.begin(); p != nonsources.end(); p++)
        result.push_back(*p);

    return result;
}

void
gr_simple_flowgraph_detail::topological_dfs_visit(gr_block_sptr block, gr_block_vector_t &output)
{
    block->detail()->set_color(gr_block_detail::GREY);

    gr_block_vector_t blocks(calc_downstream_blocks(lookup_name(block)));

    for (gr_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
        switch ((*p)->detail()->color()) {
            case gr_block_detail::WHITE:           
                topological_dfs_visit(*p, output);
                break;

            case gr_block_detail::GREY:            
                throw std::runtime_error("flow graph has loops!");

            case gr_block_detail::BLACK:
                continue;

            default:
                throw std::runtime_error("invalid color on block!");
        }
    }

    block->detail()->set_color(gr_block_detail::BLACK);
    output.push_back(block);
}
