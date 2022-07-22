#include <gnuradio/flowgraph.h>
#include <gnuradio/runtime_monitor.h>
#include <gnuradio/runtime_proxy.h>
#include <charconv>
#include <chrono>
#include <thread>

namespace gr {
runtime_proxy::sptr runtime_proxy::make(int svr_port, bool upstream)
{
    return std::make_shared<runtime_proxy>(svr_port, upstream);
}
runtime_proxy::runtime_proxy(int svr_port, bool upstream)
    : _upstream(upstream),
      _context(1),
      _server_socket(_context, zmq::socket_type::pull),
      _client_socket(_context, zmq::socket_type::push)

{
    gr::configure_default_loggers(
        d_logger,
        d_debug_logger,
        fmt::format("runtime_proxy_{}", upstream ? "upstream" : "downstream"));
    _server_socket.set(zmq::sockopt::sndhwm, 1);
    _server_socket.set(zmq::sockopt::rcvhwm, 1);
    _client_socket.set(zmq::sockopt::sndhwm, 1);
    _client_socket.set(zmq::sockopt::rcvhwm, 1);
    std::string svrendpoint = "tcp://*:" + std::to_string(svr_port);
    std::cout << "binding " << svrendpoint << std::endl;
    _server_socket.bind(svrendpoint);

    const std::string endpoint_str = _server_socket.get(zmq::sockopt::last_endpoint);
    auto colon_index = endpoint_str.find_last_of(':');
    std::string port_str =
        std::string(endpoint_str.begin() + colon_index + 1, endpoint_str.end());
    _svr_port = std::stoi(port_str);

    std::thread t([this]() {
        while (!this->_rcv_done) { // (!this->_recv_done) {
            try {
                _rcv_msg.rebuild();
                if (auto res =
                        _server_socket.recv(_rcv_msg)) //, zmq::recv_flags::dontwait))
                {
                    d_logger->debug("Got msg: {} ", _rcv_msg.to_string());
                    d_logger->debug("_rtm: {}", (void*)_rtm.get());
                    auto rtm_msg = rt_monitor_message::from_string(_rcv_msg.to_string());
                    if (_upstream) {
                        rtm_msg->set_schedid(_id);
                    }

                    if (rtm_msg && _rtm) {
                        d_logger->debug("Pushing message");
                        _rtm->push_message(rtm_msg);
                    }
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            } catch (zmq::error_t const& err) {
                this->_rcv_done = true;
            }
        }
    });
    t.detach();
}

void runtime_proxy::client_connect(const std::string& ipaddr, int cli_port)
{
    std::string cliendpoint = "tcp://" + ipaddr + ":" + std::to_string(cli_port);
    _client_socket.connect(cliendpoint);

    _cli_port = cli_port;
    _connected = true;
}
void runtime_proxy::push_message(rt_monitor_message_sptr msg)
{
    if (_connected) {
        d_logger->debug("Sending msg: {} ", msg->to_string());
        _client_socket.send(zmq::buffer(msg->to_string()), zmq::send_flags::none);
    }
    else {
        d_logger->debug("Client not yet connected");
    }
}

} // namespace gr