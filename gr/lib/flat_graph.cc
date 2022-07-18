#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <gnuradio/flat_graph.h>

namespace gr {

flat_graph::~flat_graph() {}


void flat_graph::clear()
{
    d_blocks.clear();
    graph::clear();
}

port_vector_t flat_graph::calc_used_ports(block_sptr block, bool check_inputs)
{
    port_vector_t tmp;

    // Collect all seen ports
    edge_vector_t edges = calc_connections(block, check_inputs);
    for (auto& p : edges) {
        if (check_inputs == true)
            tmp.push_back(p->dst().port());
        else
            tmp.push_back(p->src().port());
    }

    return unique_vector<port_ptr>(tmp);
}

edge_vector_t flat_graph::calc_connections(block_sptr block, bool check_inputs)
{
    edge_vector_t result;

    for (auto p : edges()) {
        if (check_inputs) {
            if (static_cast<block_endpoint>(p->dst()).block() == block)
                result.push_back(p);
        }
        else {
            if (static_cast<block_endpoint>(p->src()).block() == block)
                result.push_back(p);
        }
    }

    return result; // assumes no duplicates
}

block_vector_t flat_graph::calc_downstream_blocks(block_sptr block, port_ptr port)
{
    block_vector_t tmp;

    for (auto& p : edges())
        if (static_cast<block_endpoint>(p->src()) == block_endpoint(block, port)) {
            if (p->dst().node()) {
                tmp.push_back(static_cast<block_endpoint>(p->dst()).block());
            }
        }

    return unique_vector<block_sptr>(tmp);
}

block_vector_t flat_graph::calc_downstream_blocks(block_sptr block)
{
    block_vector_t tmp;

    for (auto& p : edges())
        if (static_cast<block_endpoint>(p->src()).block() == block)
            tmp.push_back(static_cast<block_endpoint>(p->dst()).block());

    return unique_vector<block_sptr>(tmp);
}

edge_vector_t flat_graph::calc_upstream_edges(block_sptr block)
{
    edge_vector_t result;

    for (auto p : edges())
        if (static_cast<block_endpoint>(p->dst()).block() == block)
            result.push_back(p);

    return result; // Assume no duplicates
}

bool flat_graph::has_block_p(block_sptr block)
{
    auto result = std::find(d_blocks.begin(), d_blocks.end(), block);
    return (result != d_blocks.end());
}


std::vector<block_vector_t> flat_graph::partition()
{
    std::vector<block_vector_t> result;
    block_vector_t blocks = calc_used_blocks();
    block_vector_t graph;

    while (!blocks.empty()) {
        graph = calc_reachable_blocks(blocks[0], blocks);
        assert(!graph.empty());
        result.push_back(topological_sort(graph));

        for (auto p = graph.begin(); p != graph.end(); p++)
            blocks.erase(find(blocks.begin(), blocks.end(), *p));
    }

    return result;
}

block_vector_t flat_graph::calc_reachable_blocks(block_sptr block, block_vector_t& blocks)
{
    block_vector_t result;

    // Mark all blocks as unvisited
    for (auto& b : blocks)
        block_color[b] = WHITE;

    // Recursively mark all reachable blocks
    reachable_dfs_visit(block, blocks);

    // Collect all the blocks that have been visited
    for (auto& b : blocks)
        if (block_color[b] == BLACK)
            result.push_back(b);

    return result;
}

// Recursively mark all reachable blocks from given block and block list
void flat_graph::reachable_dfs_visit(block_sptr block, block_vector_t& blocks)
{
    // Mark the current one as visited
    block_color[block] = BLACK;

    // Recurse into adjacent vertices
    block_vector_t adjacent = calc_adjacent_blocks(block, blocks);

    for (auto& b : adjacent)
        if (block_color[b] == WHITE)
            reachable_dfs_visit(b, blocks);
}

// Return a list of block adjacent to a given block along any edge
block_vector_t flat_graph::calc_adjacent_blocks(block_sptr block, block_vector_t& blocks)
{
    block_vector_t tmp;

    // Find any blocks that are inputs or outputs
    for (auto& p : edges()) {
        if (p->src().node() == block)
            tmp.push_back(static_cast<block_endpoint>(p->dst()).block());
        if (p->dst().node() == block)
            tmp.push_back(static_cast<block_endpoint>(p->src()).block());
    }

    return unique_vector<block_sptr>(tmp);
}

block_vector_t flat_graph::topological_sort(block_vector_t& blocks)
{
    block_vector_t tmp;
    block_vector_t result;
    tmp = sort_sources_first(blocks);

    // Start 'em all white
    for (auto& b : tmp)
        block_color[b] = WHITE;

    for (auto& b : tmp) {
        if (block_color[b] == WHITE)
            topological_dfs_visit(b, result);
    }

    reverse(result.begin(), result.end());
    return result;
}

block_vector_t flat_graph::sort_sources_first(block_vector_t& blocks)
{
    block_vector_t sources, nonsources, result;

    for (auto p = blocks.begin(); p != blocks.end(); p++) {
        if (source_p(*p))
            sources.push_back(*p);
        else
            nonsources.push_back(*p);
    }

    for (auto p = sources.begin(); p != sources.end(); p++)
        result.push_back(*p);

    for (auto p = nonsources.begin(); p != nonsources.end(); p++)
        result.push_back(*p);

    return result;
}

bool flat_graph::source_p(block_sptr block) { return calc_upstream_edges(block).empty(); }

void flat_graph::topological_dfs_visit(block_sptr block, block_vector_t& output)
{
    block_color[block] = GREY;
    block_vector_t blocks(calc_downstream_blocks(block));

    for (auto& b : blocks) {
        switch (block_color[b]) {
        case WHITE:
            topological_dfs_visit(b, output);
            break;

        case GREY:
            throw std::runtime_error("flow graph has loops!");

        case BLACK:
            continue;

        default:
            throw std::runtime_error("invalid color on block!");
        }
    }


    output.push_back(block);
}

using flat_graph_sptr = std::shared_ptr<flat_graph>;

} /* namespace gr */
