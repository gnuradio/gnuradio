#include <gnuradio/flowgraph.h>
#include <gnuradio/graph_utils.h>

#include <gnuradio/runtime.h>

namespace gr {


flowgraph::flowgraph(const std::string& name) { set_alias(name); }

size_t get_port_itemsize(port_ptr port)
{
    size_t size = 0;
    if (!port->connected_ports().empty()) {
        auto cp = port->connected_ports()[0];
        auto p = dynamic_cast<port_ptr>(cp);
        // use data_size since this includes vector sizing
        if (p)
            size = p->itemsize();
    }
    return size;
}

std::string get_port_format_descriptor(port_ptr port)
{
    std::string fd = "";
    if (!port->connected_ports().empty()) {
        auto cp = port->connected_ports()[0];
        auto p = dynamic_cast<port_ptr>(cp);
        if (p)
            fd = p->format_descriptor();
    }
    return fd;
}


void flowgraph::check_connections(const graph_sptr& g)
{
    // Are all non-optional ports connected to something
    for (auto& node : g->calc_used_nodes()) {
        for (auto& port : node->output_ports()) {
            if (!port->optional() && port->connected_ports().empty()) {
                throw std::runtime_error("Nothing connected to " + node->name() + ": " +
                                         port->name());
            }
        }
        for (auto& port : node->input_ports()) {
            if (!port->optional()) {

                if (port->type() == port_type_t::STREAM) {

                    if (port->connected_ports().empty()) {
                        throw std::runtime_error("Nothing connected to " + node->name() +
                                                 ": " + port->name());
                    }
                    else if (port->connected_ports().size() > 1) {
                        throw std::runtime_error("More than 1 port connected to " +
                                                 node->name() + ": " + port->name());
                    }
                }
                else if (port->type() == port_type_t::MESSAGE) {
                    if (port->connected_ports().empty()) {
                        throw std::runtime_error("Nothing connected to " + node->name() +
                                                 ": " + port->name());
                    }
                }
            }
        }
    }

    // Iteratively check the flowgraph for 0 size ports and adjust
    bool updated_sizes = true;
    while (updated_sizes) {
        updated_sizes = false;
        for (auto& node : g->calc_used_nodes()) {
            for (auto& port : node->output_ports()) {
                if (port->itemsize() == 0) {
                    // we need to propagate the itemsize from whatever it is connected to
                    auto newsize = get_port_itemsize(port);
                    auto newfd = get_port_format_descriptor(port);
                    port->set_itemsize(newsize);
                    port->set_format_descriptor(newfd);
                    updated_sizes = newsize > 0;
                }
            }
            for (auto& port : node->input_ports()) {
                if (port->itemsize() == 0) {
                    // we need to propagate the itemsize from whatever it is connected to
                    auto newsize = get_port_itemsize(port);
                    auto newfd = get_port_format_descriptor(port);
                    port->set_itemsize(newsize);
                    port->set_format_descriptor(newfd);
                    updated_sizes = newsize > 0;
                }
            }
        }
    }

    // Set any remaining 0 size ports to something
    size_t newsize = sizeof(gr_complex); // why not, does it matter
    for (auto& node : g->calc_used_nodes()) {
        for (auto& port : node->output_ports()) {
            if (port->itemsize() == 0) {
                port->set_itemsize(newsize);
                port->set_format_descriptor("c8");
            }
        }
        for (auto& port : node->input_ports()) {
            if (port->itemsize() == 0) {
                port->set_itemsize(newsize);
                port->set_format_descriptor("c8");
            }
        }
    }
}

flat_graph_sptr flowgraph::make_flat() { return flat_graph::make_flat(base()); }

void flowgraph::start()
{
    if (!d_runtime_sptr) {
        d_runtime_sptr = gr::runtime::make();
        d_runtime_sptr->initialize(base());
    }

    d_runtime_sptr->start();
}
void flowgraph::stop()
{
    if (!d_runtime_sptr) {
        throw new std::runtime_error("stop: No runtime has been created");
    }
    d_runtime_sptr->stop();
}
void flowgraph::wait()
{
    if (!d_runtime_sptr) {
        throw new std::runtime_error("wait: No runtime has been created");
    }
    d_runtime_sptr->wait();
}
void flowgraph::run()
{
    if (!d_runtime_sptr) {
        d_runtime_sptr = gr::runtime::make();
        d_runtime_sptr->initialize(base());
    }
    d_runtime_sptr->run();
}

void flowgraph::kill()
{
    if (!d_runtime_sptr) {
        throw new std::runtime_error("wait: No runtime has been created");
    }
    d_runtime_sptr->kill();
}


} // namespace gr
