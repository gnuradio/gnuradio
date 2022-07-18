#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/buffer.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
enum class buffer_cuda_type { D2D, H2D, D2H, UNKNOWN };

class buffer_cuda : public buffer
{
private:
    uint8_t* _host_buffer;
    uint8_t* _device_buffer;
    buffer_cuda_type _type = buffer_cuda_type::UNKNOWN;
    cudaStream_t stream;

public:
    using sptr = std::shared_ptr<buffer_cuda>;
    buffer_cuda(size_t num_items,
                size_t item_size,
                buffer_cuda_type type,
                std::shared_ptr<buffer_properties> buf_properties);
    ~buffer_cuda();

    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    void* read_ptr(size_t read_index);
    void* write_ptr();

    virtual void post_write(int num_items);

    virtual buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                          size_t itemsize);
};

class buffer_cuda_reader : public buffer_reader
{
public:
    buffer_cuda_reader(buffer* bufp,
                       std::shared_ptr<buffer_properties> buf_props,
                       size_t itemsize,
                       size_t read_index)
        : buffer_reader(bufp, buf_props, itemsize, read_index)
    {
    }

    virtual void post_read(int num_items);
};

class buffer_cuda_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_cuda_properties(buffer_cuda_type buffer_type_)
        : buffer_properties(), _buffer_type(buffer_type_)
    {
        _bff = buffer_cuda::make;
    }
    buffer_cuda_type buffer_type() { return _buffer_type; }
    static std::shared_ptr<buffer_properties>
    make(buffer_cuda_type buffer_type_ = buffer_cuda_type::D2D)
    {
        return std::static_pointer_cast<buffer_properties>(
            std::make_shared<buffer_cuda_properties>(buffer_type_));
    }

private:
    buffer_cuda_type _buffer_type;
};


} // namespace gr

#define CUDA_BUFFER_ARGS_H2D buffer_cuda_properties::make(buffer_cuda_type::H2D)
#define CUDA_BUFFER_ARGS_D2H buffer_cuda_properties::make(buffer_cuda_type::D2H)
#define CUDA_BUFFER_ARGS_D2D buffer_cuda_properties::make(buffer_cuda_type::D2D)
