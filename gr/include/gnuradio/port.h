#pragma once

#include <gnuradio/api.h>
#include <gnuradio/buffer.h>
#include <gnuradio/neighbor_interface.h>
#include <gnuradio/parameter_types.h>
#include <gnuradio/port_interface.h>
#include <gnuradio/scheduler_message.h>
#include <algorithm>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <utility>


namespace gr {

enum class GR_RUNTIME_API port_type_t { STREAM, MESSAGE };

enum class GR_RUNTIME_API port_direction_t {
    INPUT,
    OUTPUT,
    BIDIRECTONAL //?? can it be done
};

class block;

/**
 * @brief Base class for all ports
 *
 * Holds the necessary information to describe the port to the runtime
 *
 */
class GR_RUNTIME_API port_base : public port_interface
{

public:
    using uptr = std::unique_ptr<port_base>;
    static uptr make(const std::string& name,
                     const port_direction_t direction,
                     const param_type_t data_type = param_type_t::CFLOAT,
                     const port_type_t port_type = port_type_t::STREAM,
                     const std::vector<size_t>& shape = std::vector<size_t>{ 1 },
                     const bool optional = false,
                     const int multiplicity = 1);

    port_base(const std::string& name,
              const port_direction_t direction,
              const param_type_t data_type = param_type_t::CFLOAT,
              const port_type_t port_type = port_type_t::STREAM,
              const std::vector<size_t>& shape = std::vector<size_t>{ 1 },
              const bool optional = false,
              const int multiplicity = 1);
    port_base(const std::string& name,
              const port_direction_t direction,
              const size_t itemsize,
              const port_type_t port_type = port_type_t::STREAM,
              const bool optional = false,
              const int multiplicity = 1);

    ~port_base() override = default;

    std::string name() { return _name; }
    std::string alias() { return _alias; }
    void set_alias(const std::string& alias) { _alias = alias; }
    void set_index(int val) { _index = val; }
    int index() { return _index; }
    port_type_t type() { return _port_type; }
    param_type_t data_type() { return _data_type; }
    port_direction_t direction() { return _direction; }
    size_t data_size() { return _datasize; }
    size_t itemsize() { return _itemsize; }
    void set_itemsize(size_t itemsize) { _itemsize = itemsize; }
    std::vector<size_t> shape() { return _shape; }
    bool optional() { return _optional; }
    std::vector<port_interface_ptr>& connected_ports() { return _connected_ports; }

    void set_parent_intf(neighbor_interface_sptr intf) { _parent_intf = intf; }
    std::string format_descriptor();
    void set_format_descriptor(const std::string& fd) { _format_descriptor = fd; }
    void set_buffer(buffer_uptr&& buffer) { _buffer = std::move(buffer); }
    buffer* get_buffer() { return _buffer.get(); }
    void set_buffer_reader(buffer_reader_uptr&& rdr) { _buffer_reader = std::move(rdr); }
    buffer_reader* get_buffer_reader() { return _buffer_reader.get(); }

    void notify_msgport_message(pmtf::pmt msg);
    void notify_scheduler_action(scheduler_action_t action);
    // Inbound messages
    void push_message(scheduler_message_sptr msg) override;
    void connect(port_interface_ptr other_port);
    void disconnect(port_interface_ptr other_port);

protected:
    std::string _name;
    std::string _alias;
    port_direction_t _direction;
    param_type_t _data_type;
    port_type_t _port_type;
    int _index = -1;            // how does this get set??
    std::vector<size_t> _shape; // allow for matrices to be sent naturally across ports
    // empty shape refers to a scalar, shape=[n] same as vlen=n
    bool _optional;
    int _multiplicity; // port can be replicated as in grc
    size_t _datasize;
    size_t _itemsize; // data size across all shape
    std::string _format_descriptor = "";

    std::vector<port_interface_ptr> _connected_ports;
    neighbor_interface_sptr _parent_intf = nullptr;
    buffer_uptr _buffer = nullptr;
    buffer_reader_uptr _buffer_reader = nullptr;

    block* _parent_block = nullptr;
};

using port_ptr = port_base*;
using port_uptr = std::unique_ptr<port_base>;
using port_vector_t = std::vector<port_ptr>;


/**
 * @brief Typed port class
 *
 * Wraps the port_base class with a type to take care of all the sizing and lower level
 * properties
 *
 * @tparam T datatype to instantiate the base port class
 */
template <class T>
class GR_RUNTIME_API port : public port_base
{
public:
    using uptr = std::unique_ptr<port<T>>;
    static uptr make(const std::string& name,
                     const port_direction_t direction,
                     const std::vector<size_t>& shape = { 1 },
                     const bool optional = false,
                     const int multiplicity = 1);
    port(const std::string& name,
         const port_direction_t direction,
         const std::vector<size_t>& shape = { 1 },
         const bool optional = false,
         const int multiplicity = 1);
};


/**
 * @brief Untyped port class
 *
 * Wraps the port base class but only populates stream size info.  To be used in case of
 * e.g. head block where the underlying datatype is not used, just copied byte for byte
 *
 */
class GR_RUNTIME_API untyped_port : public port_base
{
public:
    using uptr = std::unique_ptr<untyped_port>;
    static uptr make(const std::string& name,
                     const port_direction_t direction,
                     const size_t itemsize,
                     const bool optional = false,
                     const int multiplicity = 1);
    untyped_port(const std::string& name,
                 const port_direction_t direction,
                 const size_t itemsize,
                 const bool optional = false,
                 const int multiplicity = 1);
};


/**
 * @brief Message port class
 *
 * Wraps the port_base class to provide a message port where streaming parameters are
 * absent and the type is MESSAGE
 *
 */
class GR_RUNTIME_API message_port : public port_base
{
private: //
    message_port_callback_fcn _callback_fcn;

public:
    using uptr = std::unique_ptr<message_port>;
    static uptr make(const std::string& name,
                     const port_direction_t direction,
                     const bool optional = true,
                     const int multiplicity = 1);
    message_port(const std::string& name,
                 const port_direction_t direction,
                 const bool optional = false,
                 const int multiplicity = 1);


    message_port_callback_fcn callback() { return _callback_fcn; }
    void register_callback(message_port_callback_fcn fcn) { _callback_fcn = fcn; }
    void post(pmtf::pmt msg);
    void push_message(scheduler_message_sptr msg) override;
};

using message_port_uptr = std::unique_ptr<message_port>;
using message_port_ptr = message_port*;

} // namespace gr
