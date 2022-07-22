#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include <zmq.hpp>

#include <gnuradio/buffer.h>

namespace gr {


class buffer_net_zmq_reader;

class buffer_net_zmq : public buffer
{
private:
    std::vector<uint8_t> _buffer;

    zmq::context_t _context;
    zmq::socket_t _socket;


public:
    using sptr = std::shared_ptr<buffer_net_zmq>;
    buffer_net_zmq(size_t num_items,
                   size_t item_size,
                   std::shared_ptr<buffer_properties> buffer_properties,
                   int port);
    ~buffer_net_zmq() override{};
    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    void* read_ptr(size_t index) override { return nullptr; }
    size_t space_available() override { return _num_items; }
    void* write_ptr() override { return _buffer.data(); }

    void post_write(int num_items) override
    {
        // send the data from buffer over the socket
        d_debug_logger->debug("sending {} items", num_items);
        auto res = _socket.send(zmq::buffer(write_ptr(), num_items * _item_size),
                                zmq::send_flags::none);
        d_debug_logger->debug("send returned code {}", *res);
    }

    buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                  size_t itemsize) override
    {
        // do nothing because readers will be added on zmq connect
        return nullptr;
    }
};

class buffer_net_zmq_reader : public buffer_reader
{
private:
    zmq::context_t _context;
    zmq::socket_t _socket;
    zmq::message_t _msg;
    size_t _msg_idx = 0;
    size_t _msg_size = 0;

    // Circular buffer for zmq to write into
    gr::buffer_uptr _circbuf;
    gr::buffer_reader_uptr _circbuf_rdr;

    logger_ptr d_logger;
    logger_ptr d_debug_logger;

public:
    bool _recv_done = false;
    static buffer_reader_uptr make(size_t itemsize,
                                   std::shared_ptr<buffer_properties> buf_props);
    buffer_net_zmq_reader(std::shared_ptr<buffer_properties> buf_props,
                          size_t itemsize,
                          const std::string& ipaddr,
                          int port);

    ~buffer_net_zmq_reader() override{};

    bool read_info(buffer_info_t& info) override
    {
        auto ret = _circbuf_rdr->read_info(info);
        return ret;
    }
    void* read_ptr() override { return _circbuf_rdr->read_ptr(); }

    // Tags not supported yet
    const std::vector<tag_t>& tags() const override { return _circbuf->tags(); }
    std::vector<tag_t> get_tags(size_t num_items) override { return {}; }
    void post_read(int num_items) override
    {
        d_debug_logger->debug("post_read: {}", num_items);
        _circbuf_rdr->post_read(num_items);
    }
};


class buffer_net_zmq_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_net_zmq_properties(const std::string& ipaddr, int port)
        : buffer_properties(), _ipaddr(ipaddr), _port(port)
    {
        _bff = buffer_net_zmq::make;
        _brff = buffer_net_zmq_reader::make;
    }
    ~buffer_net_zmq_properties() override{};

    static std::shared_ptr<buffer_properties> make(const std::string& ipaddr,
                                                   int port = 0)
    {
        return std::dynamic_pointer_cast<buffer_properties>(
            std::make_shared<buffer_net_zmq_properties>(ipaddr, port));
    }

    static std::shared_ptr<buffer_properties>
    make_from_params(const std::string& json_str);

    void set_port(int port) { _port = port; }
    auto port() { return _port; }
    auto ipaddr() { return _ipaddr; }

    std::string to_json() override;

private:
    std::string _ipaddr;
    int _port;
};

} // namespace gr
