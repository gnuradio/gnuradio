#include <gnuradio/graph_utils.h>

#include <gnuradio/block.h>

namespace gr {

std::pair<std::vector<graph_sptr>,
          std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>>>
graph_utils::partition(graph_sptr input_graph,
                       std::vector<std::vector<node_sptr>> nodes_vec)
{
    std::vector<graph_sptr> ret;
    edge_vector_t domain_crossings;
    std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>> crossings_with_graph;

    for (auto& nodes : nodes_vec) {

        auto g = graph::make(); // create a new subgraph
        // Go through the blocks assigned to this scheduler
        // See whether they connect to the same graph or account for a domain crossing

        graph_partition_info part_info;
        for (auto b : nodes) // for each of the blocks in the vector
        {
            // Store the block to scheduler mapping for later use
            // block_to_scheduler_map[b->id()] = sched;

            for (auto input_port : b->input_ports()) {
                auto edges = input_graph->find_edge(input_port);
                // There should only be one edge connected to an input port
                // Crossings associated with the downstream port
                if (!edges.empty()) {
                    auto e = edges[0];
                    auto other_block = e->src().node();

                    // Is the other block in our current partition
                    if (std::find(nodes.begin(), nodes.end(), other_block) !=
                        nodes.end()) {
                        g->connect(e->src(), e->dst())
                            ->set_custom_buffer(e->buf_properties());
                    }
                    else {
                        // add this edge to the list of domain crossings
                        // domain_crossings.push_back(std::make_tuple(g,e));
                        domain_crossings.push_back(e);
                    }
                }
            }
        }

        // neighbor_map is populated below
        ret.push_back(g);
    }

    int idx = 0;
    for (auto& nodes : nodes_vec) {
        auto g = ret[idx];

        // see that all the blocks in conf->blocks() are in g, and if not, add them as
        // orphan nodes

        for (auto b : nodes) // for each of the blocks in the tuple
        {
            bool connected = false;
            for (auto e : g->edges()) {
                if (e->src().node() == b || e->dst().node() == b) {
                    connected = true;
                    break;
                }
            }

            if (!connected) {
                g->add_orphan_node(b);
            }
        }

        idx++;
    }

    // For the crossing, make sure the edge that crosses the domain is included

    int crossing_index = 0;
    for (auto c : domain_crossings) {

        // Find the subgraph that holds src block
        graph_sptr src_block_graph = nullptr;
        for (auto g : ret) {
            auto blocks = g->calc_used_nodes();
            if (std::find(blocks.begin(), blocks.end(), c->src().node()) !=
                blocks.end()) {
                src_block_graph = g;
                break;
            }
        }

        // Find the subgraph that holds dst block
        graph_sptr dst_block_graph = nullptr;
        for (auto g : ret) {
            auto blocks = g->calc_used_nodes();
            if (std::find(blocks.begin(), blocks.end(), c->dst().node()) !=
                blocks.end()) {
                dst_block_graph = g;
                break;
            }
        }

        if (!src_block_graph || !dst_block_graph) {
            throw std::runtime_error("Cannot find both sides of domain crossing");
        }

        // src_block_graph->add_edge(c);
        // dst_block_graph->add_edge(c);

        // std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>> crossings_with_graph
        crossings_with_graph.push_back(
            std::make_tuple(c, src_block_graph, dst_block_graph));
        crossing_index++;
    }

    return std::make_pair(ret, crossings_with_graph);
}


void graph_utils::connect_crossings(
    std::pair<std::vector<graph_sptr>,
              std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>>>& graph_info)
{

    auto graphs = std::get<0>(graph_info);
    auto domain_crossings = std::get<1>(graph_info);

    // For the crossing, make sure the edge that crosses the domain is included
    for (auto tup : domain_crossings) {
        auto c = std::get<0>(tup);
        auto src_block_graph = std::get<1>(tup);
        auto dst_block_graph = std::get<2>(tup);

        src_block_graph->add_edge(c);
        dst_block_graph->add_edge(c);
    }
}


} // namespace gr
