#include <cuda.h>
#include <cuda_runtime.h>
#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/buffer_cuda_sm.h>

namespace gr {
buffer_cuda_sm::buffer_cuda_sm(size_t num_items,
                               size_t item_size,
                               buffer_cuda_sm_type type,
                               std::shared_ptr<buffer_properties> buf_properties)
    : gr::buffer_sm(num_items, item_size, buf_properties), _type(type)
{
    // _host_buffer.resize(_buf_size * 2); // double circular buffer
    cudaMallocHost(&_host_buffer, _buf_size);
    cudaMalloc(&_device_buffer, _buf_size);
    set_type("buffer_cuda_sm_" + std::to_string((int)_type));

    cudaStreamCreate(&stream);
}
buffer_cuda_sm::~buffer_cuda_sm()
{
    cudaFree(_device_buffer);
    cudaFree(_host_buffer);
}

buffer_uptr buffer_cuda_sm::make(size_t num_items,
                                 size_t item_size,
                                 std::shared_ptr<buffer_properties> buffer_properties)
{
    auto cbp = std::static_pointer_cast<buffer_cuda_sm_properties>(buffer_properties);
    if (cbp != nullptr) {
        return buffer_uptr(new buffer_cuda_sm(
            num_items, item_size, cbp->buffer_type(), buffer_properties));
    }
    else {
        throw std::runtime_error(
            "Failed to cast buffer properties to buffer_cuda_sm_properties");
    }
}

void* buffer_cuda_sm::read_ptr(size_t index)
{
    if (_type == buffer_cuda_sm_type::D2H) {
        return (void*)&_host_buffer[index];
    }
    else {
        return (void*)&_device_buffer[index];
    }
}
void* buffer_cuda_sm::write_ptr()
{
    if (_type == buffer_cuda_sm_type::H2D) {
        return (void*)&_host_buffer[_write_index];
    }
    else {
        return (void*)&_device_buffer[_write_index];
    }
}

void buffer_cuda_sm::post_write(int num_items)
{
    std::lock_guard<std::mutex> guard(_buf_mutex);

    size_t bytes_written = num_items * _item_size;
    size_t wi1 = _write_index;

    // num_items were written to the buffer

    if (_type == buffer_cuda_sm_type::H2D) {
        cudaMemcpyAsync(&_device_buffer[wi1],
                        &_host_buffer[wi1],
                        bytes_written,
                        cudaMemcpyHostToDevice,
                        stream);
    }
    else if (_type == buffer_cuda_sm_type::D2H) {
        cudaMemcpyAsync(&_host_buffer[wi1],
                        &_device_buffer[wi1],
                        bytes_written,
                        cudaMemcpyDeviceToHost,
                        stream);
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
    cudaStreamSynchronize(stream);
}

buffer_reader_uptr
buffer_cuda_sm::add_reader(std::shared_ptr<buffer_properties> buf_props, size_t itemsize)
{
    auto r =
        std::make_unique<buffer_cuda_sm_reader>(this, buf_props, itemsize, _write_index);
    _readers.push_back(r.get());
    return r;
}

void* buffer_cuda_sm::cuda_memcpy(void* dest, const void* src, std::size_t count)
{
    cudaError_t rc = cudaSuccess;
    rc = cudaMemcpy(dest, src, count, cudaMemcpyDeviceToDevice);
    if (rc) {
        std::ostringstream msg;
        msg << "Error performing cudaMemcpy: " << cudaGetErrorName(rc) << " -- "
            << cudaGetErrorString(rc);
        throw std::runtime_error(msg.str());
    }

    return dest;
}

void* buffer_cuda_sm::cuda_memmove(void* dest, const void* src, std::size_t count)
{
    // Would a kernel that checks for overlap and then copies front-to-back or
    // back-to-front be faster than using cudaMemcpy with a temp buffer?

    // Allocate temp buffer
    void* tempBuffer = nullptr;
    cudaError_t rc = cudaSuccess;
    rc = cudaMalloc((void**)&tempBuffer, count);
    if (rc) {
        std::ostringstream msg;
        msg << "Error allocating device buffer: " << cudaGetErrorName(rc) << " -- "
            << cudaGetErrorString(rc);
        throw std::runtime_error(msg.str());
    }

    // First copy data from source to temp buffer
    rc = cudaMemcpy(tempBuffer, src, count, cudaMemcpyDeviceToDevice);
    if (rc) {
        std::ostringstream msg;
        msg << "Error performing cudaMemcpy: " << cudaGetErrorName(rc) << " -- "
            << cudaGetErrorString(rc);
        throw std::runtime_error(msg.str());
    }

    // Then copy data from temp buffer to destination to avoid overlap
    rc = cudaMemcpy(dest, tempBuffer, count, cudaMemcpyDeviceToDevice);
    if (rc) {
        std::ostringstream msg;
        msg << "Error performing cudaMemcpy: " << cudaGetErrorName(rc) << " -- "
            << cudaGetErrorString(rc);
        throw std::runtime_error(msg.str());
    }

    cudaFree(tempBuffer);

    return dest;
}

} // namespace gr
