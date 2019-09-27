/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2013 Free Software Foundation, Inc.
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

#include <gnuradio/flowgraph.h>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace gr {

#define FLOWGRAPH_DEBUG 0

edge::~edge() {}

flowgraph_sptr make_flowgraph() { return flowgraph_sptr(new flowgraph()); }

flowgraph::flowgraph() {}

flowgraph::~flowgraph() {}

template <class T>
static std::vector<T> unique_vector(std::vector<T> v)
{
    std::vector<T> result;
    std::insert_iterator<std::vector<T>> inserter(result, result.begin());

    sort(v.begin(), v.end());
    unique_copy(v.begin(), v.end(), inserter);
    return result;
}

void flowgraph::connect(const endpoint& src, const endpoint& dst)
{
    check_valid_port(src.block()->output_signature(), src.port());
    check_valid_port(dst.block()->input_signature(), dst.port());
    check_dst_not_used(dst);
    check_type_match(src, dst);

    // Alles klar, Herr Kommissar
    d_edges.push_back(edge(src, dst));
}

void flowgraph::disconnect(const endpoint& src, const endpoint& dst)
{
    for (auto p = d_edges.begin(); p != d_edges.end(); p++) {
        if (src == p->src() && dst == p->dst()) {
            d_edges.erase(p);
            return;
        }
    }

    std::stringstream msg;
    msg << "cannot disconnect edge " << edge(src, dst) << ", not found";
    throw std::invalid_argument(msg.str());
}

void flowgraph::validate()
{
    d_blocks = calc_used_blocks();

    for (auto& d_block : d_blocks) {
        std::vector<int> used_ports;
        int ninputs, noutputs;

        if (FLOWGRAPH_DEBUG)
            std::cout << "Validating block: " << d_block << std::endl;

        used_ports = calc_used_ports(d_block, true); // inputs
        ninputs = used_ports.size();
        check_contiguity(d_block, used_ports, true); // inputs

        used_ports = calc_used_ports(d_block, false); // outputs
        noutputs = used_ports.size();
        check_contiguity(d_block, used_ports, false); // outputs

        if (!(d_block->check_topology(ninputs, noutputs))) {
            std::stringstream msg;
            msg << "check topology failed on " << d_block << " using ninputs=" << ninputs
                << ", noutputs=" << noutputs;
            throw std::runtime_error(msg.str());
        }
    }
}

void flowgraph::clear()
{
    // Boost shared pointers will deallocate as needed
    d_blocks.clear();
    d_edges.clear();
}

void flowgraph::check_valid_port(gr::io_signature::sptr sig, int port)
{
    std::stringstream msg;

    if (port < 0) {
        msg << "negative port number " << port << " is invalid";
        throw std::invalid_argument(msg.str());
    }

    int max = sig->max_streams();
    if (max != io_signature::IO_INFINITE && port >= max) {
        msg << "port number " << port << " exceeds max of ";
        if (max == 0)
            msg << "(none)";
        else
            msg << max - 1;
        throw std::invalid_argument(msg.str());
    }
}

void flowgraph::check_valid_port(const msg_endpoint& e)
{
    if (FLOWGRAPH_DEBUG)
        std::cout << "check_valid_port( " << e.block() << ", " << e.port() << ")\n";

    if (!e.block()->has_msg_port(e.port())) {
        const gr::basic_block::msg_queue_map_t& msg_map = e.block()->get_msg_map();
        std::cout << "Could not find port: " << e.port() << " in:" << std::endl;
        for (const auto& it : msg_map)
            std::cout << it.first << std::endl;
        std::cout << std::endl;
        throw std::invalid_argument("invalid msg port in connect() or disconnect()");
    }
}

void flowgraph::check_dst_not_used(const endpoint& dst)
{
    // A destination is in use if it is already on the edge list
    for (auto& d_edge : d_edges)
        if (d_edge.dst() == dst) {
            std::stringstream msg;
            msg << "destination already in use by edge " << d_edge;
            throw std::invalid_argument(msg.str());
        }
}

void flowgraph::check_type_match(const endpoint& src, const endpoint& dst)
{
    int src_size = src.block()->output_signature()->sizeof_stream_item(src.port());
    int dst_size = dst.block()->input_signature()->sizeof_stream_item(dst.port());

    if (src_size != dst_size) {
        std::stringstream msg;
        msg << "itemsize mismatch: " << src << " using " << src_size << ", " << dst
            << " using " << dst_size;
        throw std::invalid_argument(msg.str());
    }
}

basic_block_vector_t flowgraph::calc_used_blocks()
{
    basic_block_vector_t tmp;

    // make sure free standing message blocks are included
    for (auto& d_msg_edge : d_msg_edges) {
        // all msg blocks need a thread context - otherwise start() will never be called!
        // even if it is a sender that never does anything
        tmp.push_back(d_msg_edge.src().block());
        tmp.push_back(d_msg_edge.dst().block());
    }

    // Collect all blocks in the edge list
    for (auto& d_edge : d_edges) {
        tmp.push_back(d_edge.src().block());
        tmp.push_back(d_edge.dst().block());
    }

    return unique_vector<basic_block_sptr>(tmp);
}

std::vector<int> flowgraph::calc_used_ports(basic_block_sptr block, bool check_inputs)
{
    std::vector<int> tmp;

    // Collect all seen ports
    edge_vector_t edges = calc_connections(block, check_inputs);
    for (auto& edge : edges) {
        if (check_inputs == true)
            tmp.push_back(edge.dst().port());
        else
            tmp.push_back(edge.src().port());
    }

    return unique_vector<int>(tmp);
}

edge_vector_t flowgraph::calc_connections(basic_block_sptr block, bool check_inputs)
{
    edge_vector_t result;

    for (auto& d_edge : d_edges) {
        if (check_inputs) {
            if (d_edge.dst().block() == block)
                result.push_back(d_edge);
        } else {
            if (d_edge.src().block() == block)
                result.push_back(d_edge);
        }
    }

    return result; // assumes no duplicates
}

void flowgraph::check_contiguity(basic_block_sptr block,
                                 const std::vector<int>& used_ports,
                                 bool check_inputs)
{
    std::stringstream msg;

    gr::io_signature::sptr sig =
        check_inputs ? block->input_signature() : block->output_signature();

    int nports = used_ports.size();
    int min_ports = sig->min_streams();
    int max_ports = sig->max_streams();

    if (nports == 0 && min_ports == 0)
        return;

    if (nports < min_ports) {
        msg << block << ": insufficient connected "
            << (check_inputs ? "input ports " : "output ports ") << "(" << min_ports
            << " needed, " << nports << " connected)";
        throw std::runtime_error(msg.str());
    }

    if (nports > max_ports && max_ports != io_signature::IO_INFINITE) {
        msg << block << ": too many connected "
            << (check_inputs ? "input ports " : "output ports ") << "(" << max_ports
            << " allowed, " << nports << " connected)";
        throw std::runtime_error(msg.str());
    }

    if (used_ports[nports - 1] + 1 != nports) {
        for (int i = 0; i < nports; i++) {
            if (used_ports[i] != i) {
                msg << block << ": missing connection "
                    << (check_inputs ? "to input port " : "from output port ") << i;
                throw std::runtime_error(msg.str());
            }
        }
    }
}

basic_block_vector_t flowgraph::calc_downstream_blocks(basic_block_sptr block, int port)
{
    basic_block_vector_t tmp;

    for (auto& d_edge : d_edges)
        if (d_edge.src() == endpoint(block, port))
            tmp.push_back(d_edge.dst().block());

    return unique_vector<basic_block_sptr>(tmp);
}

basic_block_vector_t flowgraph::calc_downstream_blocks(basic_block_sptr block)
{
    basic_block_vector_t tmp;

    for (auto& d_edge : d_edges)
        if (d_edge.src().block() == block)
            tmp.push_back(d_edge.dst().block());

    return unique_vector<basic_block_sptr>(tmp);
}

edge_vector_t flowgraph::calc_upstream_edges(basic_block_sptr block)
{
    edge_vector_t result;

    for (auto& d_edge : d_edges)
        if (d_edge.dst().block() == block)
            result.push_back(d_edge);

    return result; // Assume no duplicates
}

bool flowgraph::has_block_p(basic_block_sptr block)
{
    basic_block_viter_t result;
    result = std::find(d_blocks.begin(), d_blocks.end(), block);
    return (result != d_blocks.end());
}

edge flowgraph::calc_upstream_edge(basic_block_sptr block, int port)
{
    edge result;

    for (auto& d_edge : d_edges) {
        if (d_edge.dst() == endpoint(block, port)) {
            result = d_edge;
            break;
        }
    }

    return result;
}

std::vector<basic_block_vector_t> flowgraph::partition()
{
    std::vector<basic_block_vector_t> result;
    basic_block_vector_t blocks = calc_used_blocks();
    basic_block_vector_t graph;

    while (!blocks.empty()) {
        graph = calc_reachable_blocks(blocks[0], blocks);
        assert(graph.size());
        result.push_back(topological_sort(graph));

        for (auto& p : graph)
            blocks.erase(find(blocks.begin(), blocks.end(), p));
    }

    return result;
}

basic_block_vector_t flowgraph::calc_reachable_blocks(basic_block_sptr block,
                                                      basic_block_vector_t& blocks)
{
    basic_block_vector_t result;

    // Mark all blocks as unvisited
    for (auto& block : blocks)
        block->set_color(basic_block::WHITE);

    // Recursively mark all reachable blocks
    reachable_dfs_visit(block, blocks);

    // Collect all the blocks that have been visited
    for (auto& block : blocks)
        if (block->color() == basic_block::BLACK)
            result.push_back(block);

    return result;
}

// Recursively mark all reachable blocks from given block and block list
void flowgraph::reachable_dfs_visit(basic_block_sptr block, basic_block_vector_t& blocks)
{
    // Mark the current one as visited
    block->set_color(basic_block::BLACK);

    // Recurse into adjacent vertices
    basic_block_vector_t adjacent = calc_adjacent_blocks(block, blocks);

    for (auto& p : adjacent)
        if (p->color() == basic_block::WHITE)
            reachable_dfs_visit(p, blocks);
}

// Return a list of block adjacent to a given block along any edge
basic_block_vector_t flowgraph::calc_adjacent_blocks(basic_block_sptr block,
                                                     basic_block_vector_t& blocks)
{
    basic_block_vector_t tmp;

    // Find any blocks that are inputs or outputs
    for (auto& d_edge : d_edges) {
        if (d_edge.src().block() == block)
            tmp.push_back(d_edge.dst().block());
        if (d_edge.dst().block() == block)
            tmp.push_back(d_edge.src().block());
    }

    return unique_vector<basic_block_sptr>(tmp);
}

basic_block_vector_t flowgraph::topological_sort(basic_block_vector_t& blocks)
{
    basic_block_vector_t tmp;
    basic_block_vector_t result;
    tmp = sort_sources_first(blocks);

    // Start 'em all white
    for (auto& p : tmp)
        p->set_color(basic_block::WHITE);

    for (auto& p : tmp) {
        if (p->color() == basic_block::WHITE)
            topological_dfs_visit(p, result);
    }

    reverse(result.begin(), result.end());
    return result;
}

basic_block_vector_t flowgraph::sort_sources_first(basic_block_vector_t& blocks)
{
    basic_block_vector_t sources, nonsources, result;

    for (auto& block : blocks) {
        if (source_p(block))
            sources.push_back(block);
        else
            nonsources.push_back(block);
    }

    for (auto& source : sources)
        result.push_back(source);

    for (auto& nonsource : nonsources)
        result.push_back(nonsource);

    return result;
}

bool flowgraph::source_p(basic_block_sptr block)
{
    return calc_upstream_edges(block).empty();
}

void flowgraph::topological_dfs_visit(basic_block_sptr block,
                                      basic_block_vector_t& output)
{
    block->set_color(basic_block::GREY);
    basic_block_vector_t blocks(calc_downstream_blocks(block));

    for (auto& block : blocks) {
        switch (block->color()) {
        case basic_block::WHITE:
            topological_dfs_visit(block, output);
            break;

        case basic_block::GREY:
            throw std::runtime_error("flow graph has loops!");

        case basic_block::BLACK:
            continue;

        default:
            throw std::runtime_error("invalid color on block!");
        }
    }

    block->set_color(basic_block::BLACK);
    output.push_back(block);
}

void flowgraph::connect(const msg_endpoint& src, const msg_endpoint& dst)
{
    check_valid_port(src);
    check_valid_port(dst);
    for (auto& d_msg_edge : d_msg_edges) {
        if (d_msg_edge.src() == src && d_msg_edge.dst() == dst) {
            throw std::runtime_error("connect called on already connected edge!");
        }
    }
    d_msg_edges.push_back(msg_edge(src, dst));
}

void flowgraph::disconnect(const msg_endpoint& src, const msg_endpoint& dst)
{
    check_valid_port(src);
    check_valid_port(dst);
    for (auto p = d_msg_edges.begin(); p != d_msg_edges.end(); p++) {
        if (p->src() == src && p->dst() == dst) {
            d_msg_edges.erase(p);
            return;
        }
    }
    throw std::runtime_error("disconnect called on non-connected edge!");
}

std::string dot_graph_fg(flowgraph_sptr fg)
{
    basic_block_vector_t blocks = fg->calc_used_blocks();
    edge_vector_t edges = fg->edges();
    msg_edge_vector_t msg_edges = fg->msg_edges();

    std::stringstream out;

    out << "digraph flowgraph {" << std::endl;

    // Define nodes and set labels
    for (auto& block : blocks) {
        out << block->unique_id() << " [ label=\"" << block->alias() << "\" ]"
            << std::endl;
    }

    // Define edges
    for (auto& edge : edges) {
        out << edge.src().block()->unique_id() << " -> "
            << edge.dst().block()->unique_id() << std::endl;
    }

    for (auto& msg_edge : msg_edges) {
        out << msg_edge.src().block()->unique_id() << " -> "
            << msg_edge.dst().block()->unique_id() << " [color=blue]" << std::endl;
    }

    out << "}" << std::endl;

    return out.str();
}

} /* namespace gr */
