#pragma once

#include <gnuradio/logger.h>
#include <zmq.hpp>
#include <thread>

namespace gr {

class runtime_monitor;
using runtime_monitor_sptr = std::shared_ptr<runtime_monitor>;

class rt_monitor_message;
using rt_monitor_message_sptr = std::shared_ptr<rt_monitor_message>;


/**
 * @brief Object for coordinating with a remote runtime
 *
 */
class runtime_proxy
{
private:
    runtime_monitor_sptr _rtm;
    bool _upstream;

    zmq::context_t _context;
    zmq::socket_t _server_socket;
    zmq::socket_t _client_socket;
    int _svr_port;
    int _cli_port;
    zmq::message_t _rcv_msg;
    bool _connected = false;
    int _id;

    logger_ptr d_logger, d_debug_logger;

    bool _rcv_done = false;

public:
    using sptr = std::shared_ptr<runtime_proxy>;
    static sptr make(int svr_port, bool upstream);
    runtime_proxy(int svr_port, bool upstream);
    virtual ~runtime_proxy()
    {
        _context.shutdown();
        _client_socket.close();
        _server_socket.close();
        _context.close();
    }
    void push_message(rt_monitor_message_sptr msg);

    void set_runtime_monitor(runtime_monitor_sptr rtm) { _rtm = rtm; }
    int id() { return _id; }
    void set_id(int id_) { _id = id_; }
    bool upstream() { return _upstream; }

    void client_connect(const std::string& ipaddr, int svr_port);

    int svr_port() { return _svr_port; }
    int cli_port() { return _cli_port; }

    void kill()
    {
        _rcv_done = true;
        // _context.shutdown();
        // _client_socket.close();
        // _server_socket.close();
        // _context.close();
    }
};
using runtime_proxy_sptr = runtime_proxy::sptr;


} // namespace gr