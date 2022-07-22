#include <gnuradio/buffer_cpu_vmcirc.h>

#include "buffer_cpu_vmcirc_mmap_shm_open.h"
#include "buffer_cpu_vmcirc_sysv_shm.h"
#include <cstring>
#include <mutex>
#include <numeric>
// Doubly mapped circular buffer class

namespace gr {

std::mutex s_vm_mutex;

buffer_uptr buffer_cpu_vmcirc::make(size_t num_items,
                                    size_t item_size,
                                    std::shared_ptr<buffer_properties> buffer_properties)
{
    auto bp = std::static_pointer_cast<buffer_cpu_vmcirc_properties>(buffer_properties);
    if (bp != nullptr) {
        switch (bp->buffer_type()) {
        case buffer_cpu_vmcirc_type::AUTO:
        case buffer_cpu_vmcirc_type::SYSV_SHM:
            return std::make_unique<buffer_cpu_vmcirc_sysv_shm>(
                num_items, item_size, buffer_properties);
        case buffer_cpu_vmcirc_type::MMAP_SHM:
            return std::make_unique<buffer_cpu_vmcirc_mmap_shm_open>(
                num_items, item_size, buffer_properties);
        default:
            throw std::runtime_error("Invalid vmcircbuf buffer_type");
        }
    }
    else {
        throw std::runtime_error(
            "Failed to cast buffer properties to buffer_cpu_vmcirc_properties");
    }
}

buffer_cpu_vmcirc::buffer_cpu_vmcirc(size_t num_items,
                                     size_t item_size,
                                     size_t granularity,
                                     std::shared_ptr<buffer_properties> buf_properties)
    : buffer(num_items, item_size, buf_properties)
{
    // This is the code from gnuradio that forces buffers to align with items

    auto min_buffer_items = granularity / std::gcd(item_size, granularity);
    if (num_items % min_buffer_items != 0)
        num_items = ((num_items / min_buffer_items) + 1) * min_buffer_items;

    // Add warning

    // Ensure that the instantiated buffer is a multiple of the granularity
    auto requested_size = num_items * item_size;
    auto npages = requested_size / granularity;
    if (requested_size != granularity * npages) {
        npages++;
    }
    auto actual_size = granularity * npages;

    // _num_items = num_items;
    _num_items = actual_size / item_size;
    _item_size = item_size;
    // _buf_size = _num_items * _item_size;
    _buf_size = actual_size;
    _write_index = 0;
}

void* buffer_cpu_vmcirc::write_ptr() { return (void*)&_buffer[_write_index]; }

void buffer_cpu_vmcirc_reader::post_read(int num_items)
{
    std::scoped_lock guard(_rdr_mutex);

    // advance the read pointer
    _read_index += num_items * _itemsize;
    if (_read_index >= _buffer->buf_size()) {
        _read_index -= _buffer->buf_size();
    }
    _total_read += num_items;
}
void buffer_cpu_vmcirc::post_write(int num_items)
{
    std::scoped_lock guard(_buf_mutex);

    unsigned int bytes_written = num_items * _item_size;

    // advance the write pointer
    _write_index += bytes_written;
    if (_write_index >= _buf_size) {
        _write_index -= _buf_size;
    }

    _total_written += num_items;
}

buffer_reader_uptr
buffer_cpu_vmcirc::add_reader(std::shared_ptr<buffer_properties> buf_props,
                              size_t itemsize)
{
    auto r = std::make_unique<buffer_cpu_vmcirc_reader>(
        this, buf_props, itemsize, _write_index);
    _readers.push_back(r.get());
    return r;
}

} // namespace gr
