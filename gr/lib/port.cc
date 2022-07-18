#include <gnuradio/port.h>

namespace gr {
port_base::uptr port_base::make(const std::string& name,
                                const port_direction_t direction,
                                const param_type_t data_type,
                                const port_type_t port_type,
                                const std::vector<size_t>& shape,
                                const bool optional,
                                const int multiplicity)
{
    return std::make_unique<port_base>(
        name, direction, data_type, port_type, shape, optional, multiplicity);
}

port_base::port_base(const std::string& name,
                     const port_direction_t direction,
                     const param_type_t data_type,
                     const port_type_t port_type,
                     const std::vector<size_t>& shape,
                     const bool optional,
                     const int multiplicity)
    : _name(name),
      _direction(direction),
      _data_type(data_type),
      _port_type(port_type),
      _shape(shape),
      _optional(optional),
      _multiplicity(multiplicity)
{
    // _type_info = param_type_info(_data_type); // might not be needed
    _datasize = parameter_functions::param_size_info(_data_type);
    _itemsize = _datasize;

    // If shape is empty or [1], then the port type is a scalar value
    // If shape has values, then the total itemsize is the product of the dimensions *
    // the scalar itemsize
    for (auto d : _shape)
        _itemsize *= d;
}

port_base::port_base(const std::string& name,
                     const port_direction_t direction,
                     const size_t itemsize,
                     const port_type_t port_type,
                     const bool optional,
                     const int multiplicity)
    : _name(name),
      _direction(direction),
      _data_type(param_type_t::UNTYPED),
      _port_type(port_type),
      _optional(optional),
      _multiplicity(multiplicity),
      _datasize(itemsize),
      _itemsize(itemsize)
{
}

std::string port_base::format_descriptor()
{
    if (!_format_descriptor.empty()) {
        return _format_descriptor;
    }
    else {
        return parameter_functions::get_format_descriptor(_data_type);
    }
}

void port_base::notify_msgport_message(pmtf::pmt msg)
{
    for (auto& p : _connected_ports) {
        if (p) {
            p->push_message(std::make_shared<msgport_message>(msg, nullptr));
        }
    }
}
void port_base::notify_scheduler_action(scheduler_action_t action)
{
    for (auto& p : _connected_ports) {
        if (p) {
            p->push_message(std::make_shared<scheduler_action>(action));
        }
    }
}

// Inbound messages
void port_base::push_message(scheduler_message_sptr msg)
{
    // push it to the queue of the owning thread
    if (_parent_intf) {
        _parent_intf->push_message(msg);
    }
    else {
        // In the case of distributed flowgraphs, the local version
        // of the remote node will get called here
        // in which case, it really needs to signal to the remote
        // flowgraph -- how to do that???
        // FIXME - make a logging message or exception
        std::cout << "port has no parent interface" << std::endl;
        // throw std::runtime_error("port has no parent interface");
    }
}

void port_base::connect(port_interface_ptr other_port)
{

    auto pred = [other_port](port_interface_ptr p) { return (p == other_port); };
    std::vector<port_interface_ptr>::iterator it =
        std::find_if(std::begin(_connected_ports), std::end(_connected_ports), pred);

    if (it == std::end(_connected_ports) || !other_port) { // allow nullptr to be added
        _connected_ports.push_back(
            other_port); // only connect if it is not already in there
    }
}

void port_base::disconnect(port_interface_ptr other_port)
{
    std::remove(_connected_ports.begin(), _connected_ports.end(), other_port);
}

template <typename T>
std::unique_ptr<port<T>> port<T>::make(const std::string& name,
                                       const port_direction_t direction,
                                       const std::vector<size_t>& shape,
                                       const bool optional,
                                       const int multiplicity)
{
    return std::make_unique<port<T>>(name, direction, shape, optional, multiplicity);
}

template <typename T>
port<T>::port(const std::string& name,
              const port_direction_t direction,
              const std::vector<size_t>& shape,
              const bool optional,
              const int multiplicity)
    : port_base(
          name,
          //    parent,
          direction,
          parameter_functions::get_param_type_from_typeinfo(std::type_index(typeid(T))),
          port_type_t::STREAM,
          shape,
          optional,
          multiplicity)
{
}

untyped_port::uptr untyped_port::make(const std::string& name,
                                      const port_direction_t direction,
                                      const size_t itemsize,
                                      const bool optional,
                                      const int multiplicity)
{
    return std::make_unique<untyped_port>(
        name, direction, itemsize, optional, multiplicity);
}
untyped_port::untyped_port(const std::string& name,
                           const port_direction_t direction,
                           const size_t itemsize,
                           const bool optional,
                           const int multiplicity)
    : port_base(name, direction, itemsize, port_type_t::STREAM, optional, multiplicity)
{
}

message_port::uptr message_port::make(const std::string& name,
                                      const port_direction_t direction,
                                      const bool optional,
                                      const int multiplicity)
{
    return std::make_unique<message_port>(name, direction, optional, multiplicity);
}
message_port::message_port(const std::string& name,
                           const port_direction_t direction,
                           const bool optional,
                           const int multiplicity)
    : port_base(name, direction, 0, port_type_t::MESSAGE, optional, multiplicity)
{
}


void message_port::post(pmtf::pmt msg)
{
    if (direction() == port_direction_t::OUTPUT)
        notify_msgport_message(msg);
    else {
        push_message(std::make_shared<msgport_message>(msg, _callback_fcn));
    }
}
void message_port::push_message(scheduler_message_sptr msg)
{
    auto m = std::static_pointer_cast<msgport_message>(msg);
    m->set_callback(callback());

    port_base::push_message(msg);
}

template class port<float>;
template class port<double>;
template class port<gr_complex>;
template class port<gr_complexd>;
template class port<int8_t>;
template class port<int16_t>;
template class port<int32_t>;
template class port<int64_t>;
template class port<uint8_t>;
template class port<uint16_t>;
template class port<uint32_t>;
template class port<uint64_t>;
template class port<bool>;

} // namespace gr