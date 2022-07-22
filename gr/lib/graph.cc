#include <gnuradio/graph.h>

namespace gr {

edge_sptr graph::connect(const node_endpoint& src, const node_endpoint& dst)
{
    if (src.port() && dst.port() && src.port()->itemsize() != dst.port()->itemsize() &&
        src.port()->itemsize() > 0 && dst.port()->itemsize() > 0) {
        std::stringstream msg;
        msg << "itemsize mismatch: " << src << " using " << src.port()->itemsize() << ", "
            << dst << " using " << dst.port()->itemsize();
        throw std::invalid_argument(msg.str());
    }

    // If not untyped ports, check that data types are the same
    // TODO

    auto newedge = edge::make(src, dst);
    _edges.push_back(newedge);
    if ((src.port() && src.port()->type() == gr::port_type_t::STREAM) ||
        (dst.port() && dst.port()->type() == gr::port_type_t::STREAM)) {
        _stream_edges.push_back(newedge);
    }

    if (!src.node() || !dst.node()) {
        if (src.node())
            add_orphan_node(src.node());
        if (dst.node())
            add_orphan_node(dst.node());
    }

    _nodes = calc_used_nodes();

    std::map<std::string, int> name_count;
    // update the block alias
    for (auto& b : _nodes) {
        // look in the map, see how many of that name exist
        // make the alias name + count;
        // increment the map
        int cnt;
        if (name_count.find(b->name()) == name_count.end()) {
            name_count[b->name()] = cnt = 0;
        }
        else {
            cnt = name_count[b->name()];
        }
        // b->set_alias(b->name() + std::to_string(cnt));
        name_count[b->name()] = cnt + 1;

        // for now, just use the name+nodeid as the alias
        b->set_alias(b->name() + "(" + std::to_string(b->id()) + ")");
    }

    // Give the underlying port objects information about the connected ports
    if (src.port())
        src.port()->connect(static_cast<port_interface_ptr>(dst.port()));
    if (dst.port())
        dst.port()->connect(static_cast<port_interface_ptr>(src.port()));

    return newedge;
}

edge_sptr graph::connect(node_sptr src_node,
                         unsigned int src_port_index,
                         node_sptr dst_node,
                         unsigned int dst_port_index)
{
    port_ptr src_port = (src_node == nullptr)
                            ? nullptr
                            : src_node->get_port(src_port_index,
                                                 port_type_t::STREAM,
                                                 port_direction_t::OUTPUT);
    // if (src_port == nullptr)
    //     throw std::invalid_argument("Source Port not found");

    port_ptr dst_port = (dst_node == nullptr)
                            ? nullptr
                            : dst_node->get_port(dst_port_index,
                                                 port_type_t::STREAM,
                                                 port_direction_t::INPUT);
    // if (dst_port == nullptr)
    //     throw std::invalid_argument("Destination port not found");

    return connect(node_endpoint(src_node, src_port), node_endpoint(dst_node, dst_port));
}

edge_sptr graph::connect(node_sptr src_node,
                         const std::string& src_port_name,
                         node_sptr dst_node,
                         const std::string& dst_port_name)
{
    port_ptr src_port =
        (src_node == nullptr) ? nullptr : src_node->get_port(src_port_name);
    // if (src_port == nullptr)
    // throw std::invalid_argument("Source Port not found");

    port_ptr dst_port =
        (dst_node == nullptr) ? nullptr : dst_node->get_port(dst_port_name);
    // if (dst_port == nullptr)
    // throw std::invalid_argument("Destination port not found");

    return connect(node_endpoint(src_node, src_port), node_endpoint(dst_node, dst_port));
}


edge_sptr graph::connect(node_sptr src_node, node_sptr dst_node)
{
    return connect(src_node, 0, dst_node, 0);
}

edge_sptr graph::connect(std::pair<node_sptr, unsigned int> src,
                         std::pair<node_sptr, unsigned int> dst)
{
    return connect(src.first, src.second, dst.first, dst.second);
}

edge_sptr graph::connect(std::pair<node_sptr, const std::string&> src,
                         std::pair<node_sptr, const std::string&> dst)
{
    return connect(src.first, src.second, dst.first, dst.second);
}

edge_vector_t graph::connect(const std::vector<std::pair<node_sptr, unsigned int>>& pairs)
{
    if (pairs.size() < 2) {
        throw std::runtime_error("connect: must call with 2 or more node/index pairs");
    }

    edge_vector_t ret;
    auto last_node = pairs[0].first;
    auto last_index = pairs[0].second;
    for (size_t i = 1; i < pairs.size(); i++) {
        ret.push_back(connect(last_node, last_index, pairs[i].first, pairs[i].second));
        last_node = pairs[i].first;
        last_index = pairs[i].second;
    }

    return ret;
}

edge_vector_t graph::connect(const std::vector<node_sptr>& nodes)
{
    if (nodes.size() < 2) {
        throw std::runtime_error("connect: must call with 2 or more nodes");
    }

    edge_vector_t ret;
    auto last_node = nodes[0];
    for (size_t i = 1; i < nodes.size(); i++) {
        ret.push_back(connect(last_node, nodes[i]));
        last_node = nodes[i];
    }

    return ret;
}

void graph::add_orphan_node(node_sptr orphan_node)
{
    _orphan_nodes.push_back(orphan_node);
}

node_vector_t graph::calc_used_nodes()
{
    node_vector_t tmp;

    // Collect all blocks in the edge list
    for (auto& p : edges()) {
        if (p->src().node())
            tmp.push_back(p->src().node());
        if (p->dst().node())
            tmp.push_back(p->dst().node());
    }
    for (auto n : _orphan_nodes) {
        tmp.push_back(n);
    }

    return unique_vector<node_sptr>(tmp);
}

node_vector_t graph::all_nodes()
{
    node_vector_t tmp;

    // Collect all blocks in the edge list
    for (auto& n : nodes()) {
        tmp.push_back(n);
    }
    for (auto n : _orphan_nodes) {
        tmp.push_back(n);
    }

    return unique_vector<node_sptr>(tmp);
}

edge_vector_t graph::find_edge(port_ptr port)
{
    edge_vector_t ret;
    for (auto& e : edges()) {
        if (e->src().port() == port)
            ret.push_back(e);

        if (e->dst().port() == port)
            ret.push_back(e);
    }

    // TODO: check optional flag
    // msg ports or optional streaming ports might not be connected
    // if (ret.empty())
    //     throw std::invalid_argument("edge not found");

    return ret;
}

void graph::add_edge(edge_sptr edge)
{
    // TODO: check that edge is not already in the graph
    _edges.push_back(edge);
    if ((edge->src().port() && edge->src().port()->type() == gr::port_type_t::STREAM) ||
        (edge->dst().port() && edge->dst().port()->type() == gr::port_type_t::STREAM)) {
        _stream_edges.push_back(edge);
    }
}

} // namespace gr
