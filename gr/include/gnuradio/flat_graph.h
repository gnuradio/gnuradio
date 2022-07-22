#pragma once

#include <gnuradio/block.h>
#include <gnuradio/graph.h>
#include <gnuradio/hier_block.h>

namespace gr {

/**
 * @brief Endpoint consisting only of Blocks
 *
 */
class block_endpoint : public node_endpoint
{
private:
    block_sptr d_block;

public:
    block_endpoint(block_sptr block, port_ptr port)
        : node_endpoint(block, port), d_block(block)
    {
    }

    block_endpoint(const node_endpoint& n) : node_endpoint(n) {}

    block_sptr block() { return std::dynamic_pointer_cast<gr::block>(this->node()); }
};

/**
 * @brief Flattened Graph class
 *
 * Graph that contains only blocks, and all the topological logic to determine cycles and
 * connectivity
 *
 */
class flat_graph : public graph
{
    static constexpr const char* BLOCK_COLOR_KEY = "color";
    enum vcolor { WHITE, GREY, BLACK };
    enum io { INPUT, OUTPUT };

public:
    void clear() override;
    flat_graph() {}
    // using sptr = std::shared_ptr<flat_graph>;
    ~flat_graph() override;

    block_vector_t calc_used_blocks()
    {
        block_vector_t tmp;

        // Collect all blocks in the edge list
        for (auto& p : edges()) {
            // if both ends of the edge belong to this graph
            if (std::find(_nodes.begin(), _nodes.end(), p->src().node()) !=
                    _nodes.end() &&
                std::find(_nodes.begin(), _nodes.end(), p->dst().node()) !=
                    _nodes.end()) {

                auto src_ptr = std::dynamic_pointer_cast<block>(p->src().node());
                auto dst_ptr = std::dynamic_pointer_cast<block>(p->dst().node());

                // auto src_hier_block =
                //     std::dynamic_pointer_cast<gr::hier_block>(p->src().node());
                // auto dst_hier_block =
                //     std::dynamic_pointer_cast<gr::hier_block>(p->dst().node());

                if (src_ptr != nullptr) { //} && !src_hier_block) {
                    tmp.push_back(src_ptr);
                }
                if (dst_ptr != nullptr) { //}) && !dst_hier_block) {
                    tmp.push_back(dst_ptr);
                }
            }
        }

        for (auto n : _orphan_nodes) {
            // auto hier_ptr = std::dynamic_pointer_cast<gr::hier_block>(n);
            // if (!hier_ptr) {
            tmp.push_back(std::static_pointer_cast<block>(n));
            // }
        }


        return unique_vector<block_sptr>(tmp);
    }

    block_vector_t calc_used_hier_blocks()
    {
        block_vector_t tmp;

        // Collect all blocks in the edge list
        for (auto& p : edges()) {
            // if both ends of the edge belong to this graph
            if (std::find(_nodes.begin(), _nodes.end(), p->src().node()) !=
                    _nodes.end() &&
                std::find(_nodes.begin(), _nodes.end(), p->dst().node()) !=
                    _nodes.end()) {

                auto src_ptr = std::dynamic_pointer_cast<block>(p->src().node());
                auto dst_ptr = std::dynamic_pointer_cast<block>(p->dst().node());

                auto src_hier_block =
                    std::dynamic_pointer_cast<gr::hier_block>(p->src().node());
                auto dst_hier_block =
                    std::dynamic_pointer_cast<gr::hier_block>(p->dst().node());

                if (src_ptr != nullptr && src_hier_block) {
                    tmp.push_back(src_ptr);
                }
                if (dst_ptr != nullptr && dst_hier_block) {
                    tmp.push_back(dst_ptr);
                }
            }
        }

        for (auto n : _orphan_nodes) {
            auto hier_ptr = std::dynamic_pointer_cast<gr::hier_block>(n);
            if (hier_ptr) {
                tmp.push_back(std::static_pointer_cast<block>(n));
            }
        }


        return unique_vector<block_sptr>(tmp);
    }

    static std::shared_ptr<flat_graph> make_flat(graph_sptr g)
    {
        std::map<std::shared_ptr<gr::hier_block>, bool> hier_block_map;
        auto fg = std::make_shared<flat_graph>();
        for (auto e : g->edges()) {
            bool message_connection = e->src().port()->type() == port_type_t::MESSAGE;
            // connect only if both sides of the edge are in this graph
            if (std::find(g->nodes().begin(), g->nodes().end(), e->src().node()) !=
                    g->nodes().end() &&
                std::find(g->nodes().begin(), g->nodes().end(), e->dst().node()) !=
                    g->nodes().end()) {

                auto a = std::dynamic_pointer_cast<gr::block>(e->src().node());
                auto b = std::dynamic_pointer_cast<gr::block>(e->dst().node());

                auto src_hier_block =
                    std::dynamic_pointer_cast<gr::hier_block>(e->src().node());
                auto dst_hier_block =
                    std::dynamic_pointer_cast<gr::hier_block>(e->dst().node());

                // the dst side of the edge is a hier block
                if (dst_hier_block) {

                    // find the input ports that originate from dst().port()
                    for (auto& hbe : dst_hier_block->input_edges()) {
                        if (hbe->src().port() == e->dst().port() && !message_connection) {
                            // connect with the original source port
                            fg->connect(e->src(), hbe->dst())
                                ->set_custom_buffer(e->buf_properties());

                            e->src().port()->disconnect(e->dst().port());
                            e->dst().port()->disconnect(e->src().port());

                            hbe->dst().port()->disconnect(hbe->src().port());
                        }
                    }

                    // if this hier block has not yet been handled
                    if (!hier_block_map.count(dst_hier_block)) {
                        // connect up the rest of the hier block
                        for (auto& hbe : dst_hier_block->edges()) {
                            // pad connections are INPUT-->INPUT and OUTPUT-->OUTPUT
                            if (!(hbe->src().port()->direction() ==
                                      port_direction_t::INPUT ||
                                  hbe->dst().port()->direction() ==
                                      port_direction_t::OUTPUT) &&
                                !message_connection) {
                                // then we have an internal connection that needs to be
                                // replicated
                                hbe->src().port()->disconnect(hbe->dst().port());
                                fg->connect(hbe->src(), hbe->dst())
                                    ->set_custom_buffer(hbe->buf_properties());
                            }
                        }
                        hier_block_map[dst_hier_block] = true;
                    }
                }
                if (src_hier_block) {

                    // find the input ports that originate from src().port()
                    for (auto& hbe : src_hier_block->output_edges()) {
                        if (hbe->dst().port() == e->src().port() && !message_connection) {
                            // connect with the original source port
                            fg->connect(hbe->src(), e->dst())
                                ->set_custom_buffer(e->buf_properties());

                            e->dst().port()->disconnect(e->src().port());
                            e->src().port()->disconnect(e->dst().port());
                            hbe->src().port()->disconnect(hbe->dst().port());
                        }
                    }

                    // if this hier block has not yet been handled
                    if (!hier_block_map.count(src_hier_block)) {
                        // connect up the rest of the hier block
                        for (auto& hbe : src_hier_block->edges()) {
                            // pad connections are INPUT-->INPUT and OUTPUT-->OUTPUT
                            if (!(hbe->src().port()->direction() ==
                                      port_direction_t::INPUT ||
                                  hbe->dst().port()->direction() ==
                                      port_direction_t::OUTPUT) &&
                                !message_connection) {
                                // then we have an internal connection that needs to be
                                // replicated
                                hbe->src().port()->disconnect(hbe->dst().port());
                                fg->connect(hbe->src(), hbe->dst())
                                    ->set_custom_buffer(hbe->buf_properties());
                            }
                        }
                        hier_block_map[src_hier_block] = true;
                    }
                }
                if ((!src_hier_block && !dst_hier_block) || message_connection) {
                    fg->connect(e->src(), e->dst())
                        ->set_custom_buffer(e->buf_properties());
                }
            }
            else { // edge is a pathway into another domain
                fg->add_edge(e);
            }
        }
        for (auto o : g->orphan_nodes()) {
            fg->add_orphan_node(o);
        }

        return fg;
    }

    block_vector_t calc_downstream_blocks(block_sptr block, port_ptr port);

protected:
    block_vector_t d_blocks;

    port_vector_t calc_used_ports(block_sptr block, bool check_inputs);
    edge_vector_t calc_upstream_edges(block_sptr block);
    bool has_block_p(block_sptr block);

private:
    edge_vector_t calc_connections(block_sptr block,
                                   bool check_inputs); // false=use outputs
    block_vector_t calc_downstream_blocks(block_sptr block);
    block_vector_t calc_reachable_blocks(block_sptr blk, block_vector_t& blocks);
    void reachable_dfs_visit(block_sptr blk, block_vector_t& blocks);
    block_vector_t calc_adjacent_blocks(block_sptr blk, block_vector_t& blocks);
    block_vector_t sort_sources_first(block_vector_t& blocks);
    bool source_p(block_sptr block);
    void topological_dfs_visit(block_sptr blk, block_vector_t& output);

    std::vector<block_vector_t> partition();
    block_vector_t topological_sort(block_vector_t& blocks);

    std::map<block_sptr, int> block_color;
}; // namespace gr

using flat_graph_sptr = std::shared_ptr<flat_graph>;

} // namespace gr
