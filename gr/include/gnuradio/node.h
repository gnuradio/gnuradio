#pragma once

#include <algorithm>
#include <vector>

#include <gnuradio/api.h>
#include <gnuradio/logger.h>
#include <gnuradio/nodeid_generator.h>
#include <gnuradio/port.h>
#include <gnuradio/rpc_client_interface.h>


namespace gr {

using nodeid_t = uint32_t;

/**
 * @brief Base class for node
 *
 * The node class represents all things that can be connected together in graphs.  Nodes
 * should not be instantiated directly but through the derived classes (e.g. block or
 * graph)
 *
 * Nodes hold ports, have a name, alias, and universal ID, but that's about it
 *
 * Ports are added after construction of the node to simplify the constructor
 *
 */
class GR_RUNTIME_API node : public std::enable_shared_from_this<node>
{
protected:
    std::string d_name;
    std::string d_alias;
    nodeid_t d_id; // Unique number given to block from runtime
    std::vector<port_uptr> d_ports;
    std::vector<port_ptr> d_all_ports;
    std::vector<port_ptr> d_input_ports;
    std::vector<port_ptr> d_output_ports;

    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    std::string d_rpc_name = "";
    rpc_client_interface_sptr d_rpc_client = nullptr;

public:
    node() : d_name("") {}
    node(const std::string& name)
        : d_name(name),
          d_alias(name),
          d_logger(std::make_shared<gr::logger>(name)),
          d_debug_logger(std::make_shared<logger_ptr::element_type>(name + " (debug)"))
    {
        d_id = nodeid_generator::get_id();
    }
    virtual ~node() {}
    using sptr = std::shared_ptr<node>;

    void add_port(port_uptr&& p);
    void add_port(message_port_uptr&& p);

    std::vector<port_ptr>& all_ports() { return d_all_ports; }
    std::vector<port_ptr>& input_ports() { return d_input_ports; }
    std::vector<port_ptr>& output_ports() { return d_output_ports; }
    std::vector<port_ptr> input_stream_ports();
    std::vector<port_ptr> output_stream_ports();
    message_port_ptr input_message_port(const std::string& port_name);

    std::string name() const { return d_name; };
    std::string alias() const { return d_alias; }

    logger_ptr logger() const { return d_logger; }
    logger_ptr debug_logger() const { return d_debug_logger; }

    uint32_t id() { return d_id; }
    void set_alias(std::string alias);
    void set_id(uint32_t id) { d_id = id; }

    port_ptr get_port(const std::string& name);
    message_port_ptr get_message_port(const std::string& name);
    message_port_ptr get_first_message_port(port_direction_t direction);
    port_ptr get_port(unsigned int index, port_type_t type, port_direction_t direction);

    void set_rpc(const std::string& rpc_name, rpc_client_interface_sptr rpc_client)
    {
        d_rpc_name = rpc_name;
        d_rpc_client = rpc_client;
    }
    rpc_client_interface_sptr rpc_client() { return d_rpc_client; }
    std::string rpc_name() { return d_rpc_name; }
    bool is_rpc() { return !(d_rpc_client == nullptr || !d_rpc_name.empty()); }
};

using node_sptr = node::sptr;
using node_vector_t = std::vector<node_sptr>;

} // namespace gr
