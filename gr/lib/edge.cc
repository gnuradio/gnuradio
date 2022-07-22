#include <gnuradio/edge.h>

namespace gr {

node_endpoint::node_endpoint() : endpoint(){};
node_endpoint::node_endpoint(node_sptr node, port_ptr port)
    : endpoint<node_sptr, port_ptr>(node, port), d_node(node), d_port(port)
{
}

node_endpoint::node_endpoint(const node_endpoint& n)
    : endpoint<node_sptr, port_ptr>(n.node(), n.port())
{
    d_node = n.node();
    d_port = n.port();
}

node_sptr node_endpoint::node() const { return d_node; }
port_ptr node_endpoint::port() const { return d_port; }
std::string node_endpoint::identifier() const
{
    if (d_node)
        return d_node->alias() + ":" + d_port->name();
    else
        return "NULL";
};

edge::edge(const node_endpoint& src, const node_endpoint& dst) : _src(src), _dst(dst) {}
edge::edge(node_sptr src_blk, port_ptr src_port, node_sptr dst_blk, port_ptr dst_port)
    : _src(node_endpoint(src_blk, src_port)), _dst(node_endpoint(dst_blk, dst_port))
{
}

node_endpoint edge::src() const { return _src; }
node_endpoint edge::dst() const { return _dst; }

std::string edge::identifier() const
{
    return _src.identifier() + "->" + _dst.identifier();
}

size_t edge::itemsize() const
{
    if (_src.port())
        return _src.port()->itemsize();
    else
        return _dst.port()->itemsize();
}

bool edge::has_custom_buffer()
{
    if (_buffer_properties) {
        return _buffer_properties->factory() != nullptr;
    }
    else {
        return false;
    }
}

buffer_factory_function edge::buffer_factory() { return _buffer_properties->factory(); }
buffer_reader_factory_function edge::buffer_reader_factory()
{
    return _buffer_properties->reader_factory();
}
std::shared_ptr<buffer_properties> edge::buf_properties() { return _buffer_properties; }

} // namespace gr
