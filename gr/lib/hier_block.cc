#include <gnuradio/hier_block.h>

namespace gr {
hier_block::hier_block(const std::string& name, const std::string& module)
    : block(name), s_module(module)
{
    _is_hier = true;
    _graph = std::make_shared<gr::graph>(name);
    // This bit of magic ensures that self() works in the constructors of derived
    // classes.
    gnuradio::detail::sptr_magic::create_and_stash_initial_sptr(this);
}

edge_sptr hier_block::connect(const node_endpoint& src, const node_endpoint& dst)
{
    if (src.node().get() == this) { // connecting to self input ports
        _input_edges.push_back(edge::make(src, dst));
    }
    else if (dst.node().get() == this) { // connection to self output ports
        _output_edges.push_back(edge::make(src, dst));
    }
    else // internal connection
    {
        return _graph->connect(src, dst);
    }

    return nullptr; // graph::connect(src, dst);
}


edge_sptr hier_block::connect(node_sptr src_node,
                              unsigned int src_port_index,
                              node_sptr dst_node,
                              unsigned int dst_port_index)
{
    auto src_direction = port_direction_t::OUTPUT;
    auto dst_direction = port_direction_t::INPUT;
    if (src_node.get() == this) {
        src_direction = port_direction_t::INPUT;
    }

    if (dst_node.get() == this) {
        dst_direction = port_direction_t::OUTPUT;
    }

    port_ptr src_port =
        (src_node == nullptr)
            ? nullptr
            : src_node->get_port(src_port_index,
                                 port_type_t::STREAM,
                                 src_direction); // for hier block it is reversed

    port_ptr dst_port =
        (dst_node == nullptr)
            ? nullptr
            : dst_node->get_port(dst_port_index, port_type_t::STREAM, dst_direction);

    return this->connect(node_endpoint(src_node, src_port),
                         node_endpoint(dst_node, dst_port));
}
} // namespace gr