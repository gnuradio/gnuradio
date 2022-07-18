#pragma once

#include <gnuradio/block.h>
#include <gnuradio/graph.h>
#include <pmtf/wrap.hpp>

namespace gr {
class GR_RUNTIME_API hier_block : public block
{
private:
    graph_sptr _graph; // hasa graph
    std::vector<edge_sptr> _input_edges;
    std::vector<edge_sptr> _output_edges;

    const std::string s_module;

public:
    hier_block(const std::string& name, const std::string& module = "");
    virtual ~hier_block() { gnuradio::detail::sptr_magic::cancel_initial_sptr(this); }
    node_sptr self() { return shared_from_this(); }
    // add_port becomes public only for hier_block
    edge_sptr connect(const node_endpoint& src, const node_endpoint& dst);
    edge_sptr connect(node_sptr src_node,
                      unsigned int src_port_index,
                      node_sptr dst_node,
                      unsigned int dst_port_index);

    auto edges() { return _graph->edges(); }
    std::vector<edge_sptr>& input_edges() { return _input_edges; }
    std::vector<edge_sptr>& output_edges() { return _output_edges; }
};
} // namespace gr
