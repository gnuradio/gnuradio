#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/buffer_cpu_host.h>

namespace gr {
buffer_cpu_host::buffer_cpu_host(size_t num_items,
                               size_t item_size,
                               buffer_cpu_host_type type,
                               std::shared_ptr<buffer_properties> buf_properties)
    : gr::buffer_sm(num_items, item_size, buf_properties), _transfer_type(type)
{
    static std::vector<uint8_t> __host_buffer(_buf_size);
    static std::vector<uint8_t> __device_buffer(_buf_size);

    _host_buffer = __host_buffer.data();
    _device_buffer = __device_buffer.data();

    set_type("buffer_cpu_host_" + std::to_string((int)_transfer_type));
}

buffer_uptr buffer_cpu_host::make(size_t num_items,
                                 size_t item_size,
                                 std::shared_ptr<buffer_properties> buffer_properties)
{
    auto cbp = std::static_pointer_cast<buffer_cpu_host_properties>(buffer_properties);
    if (cbp != nullptr) {
        return buffer_uptr(new buffer_cpu_host(
            num_items, item_size, cbp->buffer_type(), buffer_properties));
    }
    else {
        throw std::runtime_error(
            "Failed to cast buffer properties to buffer_cpu_host_properties");
    }
}

void* buffer_cpu_host::read_ptr(size_t index)
{
    if (_transfer_type == buffer_cpu_host_type::D2H) {
        return (void*)&_host_buffer[index];
    }
    else {
        return (void*)&_device_buffer[index];
    }
}
void* buffer_cpu_host::write_ptr()
{
    if (_transfer_type == buffer_cpu_host_type::H2D) {
        return (void*)&_host_buffer[_write_index];
    }
    else {
        return (void*)&_device_buffer[_write_index];
    }
}

void buffer_cpu_host::post_write(int num_items)
{
    std::lock_guard<std::mutex> guard(_buf_mutex);

    size_t bytes_written = num_items * _item_size;
    size_t wi1 = _write_index;

    // num_items were written to the buffer

    if (_transfer_type == buffer_cpu_host_type::H2D) {
        memcpy(&_device_buffer[wi1],
                        &_host_buffer[wi1],
                        bytes_written);
    }
    else if (_transfer_type == buffer_cpu_host_type::D2H) {
        memcpy(&_host_buffer[wi1],
                        &_device_buffer[wi1],
                        bytes_written);
    }

    // advance the write pointer
    _write_index += bytes_written;
    if (_write_index == _buf_size) {
        _write_index = 0;
    }
    if (_write_index > _buf_size) {
        throw std::runtime_error("buffer_sm: Wrote too far into buffer");
    }
    _total_written += num_items;
}

buffer_reader_uptr
buffer_cpu_host::add_reader(std::shared_ptr<buffer_properties> buf_props, size_t itemsize)
{
    auto r =
        std::make_unique<buffer_cpu_host_reader>(this, buf_props, itemsize, _write_index);
    _readers.push_back(r.get());
    return r;
}


} // namespace gr
