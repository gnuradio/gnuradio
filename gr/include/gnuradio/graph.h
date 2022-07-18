#pragma once

#include <gnuradio/edge.h>

namespace gr {

template <class T>
static std::vector<T> unique_vector(std::vector<T> v)
{
    std::vector<T> result;
    std::insert_iterator<std::vector<T>> inserter(result, result.begin());

    sort(v.begin(), v.end());
    unique_copy(v.begin(), v.end(), inserter);
    return result;
}

/**
 * @brief Represents a set of ports connected by edges
 *
 */

class graph : public node
{
protected:
    node_vector_t _nodes;
    edge_vector_t _edges;
    edge_vector_t _stream_edges;
    node_vector_t _orphan_nodes;

public:
    using sptr = std::shared_ptr<graph>;
    static sptr make() { return std::make_shared<graph>(); }
    graph() : node() {}
    graph(const std::string& name) : node(name) {}
    ~graph() override {}
    std::shared_ptr<graph> base()
    {
        return std::dynamic_pointer_cast<graph>(shared_from_this());
    }
    edge_vector_t& edges() { return _edges; }
    edge_vector_t& stream_edges() { return _stream_edges; }
    node_vector_t& orphan_nodes() { return _orphan_nodes; }
    node_vector_t& nodes() { return _nodes; }
    node_vector_t all_nodes();
    virtual edge_sptr connect(const node_endpoint& src, const node_endpoint& dst);
    virtual edge_sptr connect(node_sptr src_node,
                              unsigned int src_port_index,
                              node_sptr dst_node,
                              unsigned int dst_port_index);
    edge_sptr connect(node_sptr src_node, node_sptr dst_node);
    edge_sptr connect(std::pair<node_sptr, unsigned int>,
                      std::pair<node_sptr, unsigned int>);
    edge_sptr connect(std::pair<node_sptr, const std::string&>,
                      std::pair<node_sptr, const std::string&>);
    edge_vector_t connect(const std::vector<std::pair<node_sptr, unsigned int>>&);
    edge_vector_t connect(const std::vector<node_sptr>&);
    edge_sptr connect(node_sptr src_node,
                      const std::string& src_port_name,
                      node_sptr dst_node,
                      const std::string& dst_port_name);
    void disconnect(const node_endpoint& src, const node_endpoint& dst){};
    virtual void validate(){};
    virtual void clear(){};
    void add_orphan_node(node_sptr orphan_node);
    void add_edge(edge_sptr edge);

    // }
    node_vector_t calc_used_nodes();
    edge_vector_t find_edge(port_ptr port);
};

using graph_sptr = std::shared_ptr<graph>;


} // namespace gr
