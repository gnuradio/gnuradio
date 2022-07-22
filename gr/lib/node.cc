#include <gnuradio/node.h>


namespace gr {
void node::add_port(port_uptr&& p)
{
    d_ports.push_back(std::move(p));
    auto ptr = d_ports.back().get();
    d_all_ports.push_back(ptr);
    if (ptr->direction() == port_direction_t::INPUT) {
        if (ptr->type() == port_type_t::STREAM)
            ptr->set_index(input_stream_ports().size());
        // TODO: do message ports have an index??

        d_input_ports.push_back(ptr);
    }
    else if (ptr->direction() == port_direction_t::OUTPUT) {
        if (ptr->type() == port_type_t::STREAM)
            ptr->set_index(output_stream_ports().size());

        d_output_ports.push_back(ptr);
    }
}

void node::add_port(message_port_uptr&& p)
{
    port_uptr p2(static_cast<port_ptr>(p.release()));
    add_port(std::move(p2));
}

std::vector<port_ptr> node::input_stream_ports()
{
    std::vector<port_ptr> result;
    for (auto& p : d_input_ports)
        if (p->type() == port_type_t::STREAM)
            result.push_back(p);

    return result;
}
std::vector<port_ptr> node::output_stream_ports()
{
    std::vector<port_ptr> result;
    for (auto& p : d_output_ports)
        if (p->type() == port_type_t::STREAM)
            result.push_back(p);

    return result;
}

message_port_ptr node::input_message_port(const std::string& port_name)
{
    message_port_ptr result = nullptr;
    for (auto& p : d_input_ports)
        if (p->type() == port_type_t::MESSAGE && p->name() == port_name)
            result = static_cast<message_port*>(p);

    return result;
}

void node::set_alias(std::string alias)
{
    d_alias = alias;

    // Instantiate the loggers when the alias is set
    gr::configure_default_loggers(d_logger, d_debug_logger, alias);
}

port_ptr node::get_port(const std::string& name)
{
    auto pred = [name](port_ptr p) { return (p->name() == name); };
    std::vector<port_ptr>::iterator it =
        std::find_if(std::begin(d_all_ports), std::end(d_all_ports), pred);

    if (it != std::end(d_all_ports)) {
        return *it;
    }
    else {
        // port was not found
        return nullptr;
    }
}

message_port_ptr node::get_message_port(const std::string& name)
{
    auto p = get_port(name);

    // could be null if the requested port is not a message port
    return dynamic_cast<message_port*>(p);
}

message_port_ptr node::get_first_message_port(port_direction_t direction)
{
    auto pred = [direction](port_ptr p) {
        return (p->type() == port_type_t::MESSAGE && p->direction() == direction);
    };

    std::vector<port_ptr>::iterator it =
        std::find_if(std::begin(d_all_ports), std::end(d_all_ports), pred);

    if (it != std::end(d_all_ports)) {
        return dynamic_cast<message_port*>(*it);
    }
    else {
        // port was not found
        return nullptr;
    }
}

port_ptr node::get_port(unsigned int index, port_type_t type, port_direction_t direction)
{
    auto pred = [index, type, direction](port_ptr p) {
        return (p->type() == type && p->direction() == direction &&
                p->index() == (int)index);
    };
    std::vector<port_ptr>::iterator it =
        std::find_if(std::begin(d_all_ports), std::end(d_all_ports), pred);

    if (it != std::end(d_all_ports)) {
        return *it;
    }
    else {
        // port was not found
        return nullptr;
    }
}
} // namespace gr