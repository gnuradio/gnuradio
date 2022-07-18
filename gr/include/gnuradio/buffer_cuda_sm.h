#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/buffer_sm.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
enum class buffer_cuda_sm_type { D2D, H2D, D2H, UNKNOWN };

class buffer_cuda_sm : public buffer_sm
{
private:
    uint8_t* _host_buffer;
    uint8_t* _device_buffer;
    buffer_cuda_sm_type _type = buffer_cuda_sm_type::UNKNOWN;
    cudaStream_t stream;

public:
    using sptr = std::shared_ptr<buffer_cuda_sm>;
    buffer_cuda_sm(size_t num_items,
                   size_t item_size,
                   buffer_cuda_sm_type type,
                   std::shared_ptr<buffer_properties> buf_properties);
    ~buffer_cuda_sm();

    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    void* read_ptr(size_t read_index);
    void* write_ptr();
    buffer_cuda_sm_type type() { return _type; }

    virtual void post_write(int num_items);

    virtual buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                          size_t itemsize);

    static void* cuda_memcpy(void* dest, const void* src, std::size_t count);
    static void* cuda_memmove(void* dest, const void* src, std::size_t count);

    virtual bool output_blocked_callback(bool force = false) override
    {
        switch (_type) {
        case buffer_cuda_sm_type::H2D:
            return output_blocked_callback_logic(force, std::memmove);
        case buffer_cuda_sm_type::D2D:
        case buffer_cuda_sm_type::D2H:
            return output_blocked_callback_logic(force, cuda_memmove);
        default:
            return false;
        }
    }
};

class buffer_cuda_sm_reader : public buffer_sm_reader
{
private:
    // logger_ptr d_logger;
    // logger_ptr d_debug_logger;

    buffer_cuda_sm* _buffer_cuda_sm;

public:
    buffer_cuda_sm_reader(buffer_cuda_sm* buffer,
                          std::shared_ptr<buffer_properties> buf_props,
                          size_t itemsize,
                          size_t read_index)
        : buffer_sm_reader(buffer, itemsize, buf_props, read_index)
    {
        _buffer_cuda_sm = buffer;
        // gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_cuda_sm");
    }

    // virtual void post_read(int num_items);

    virtual bool input_blocked_callback(size_t items_required) override
    {
        // Only singly mapped buffers need to do anything with this callback
        // std::scoped_lock guard(*(_buffer->mutex()));
        std::lock_guard<std::mutex> guard(*(_buffer->mutex()));

        auto items_avail = items_available();

        // GR_LOG_DEBUG(d_debug_logger,
        //              "input_blocked_callback: items_avail {}, _read_index {}, "
        //              "_write_index {}, items_required {}",
        //              items_avail,
        //              _read_index,
        //              _buffer->write_index(),
        //              items_required);

        // GR_LOG_DEBUG(d_debug_logger,
        //              "input_blocked_callback: total_written {}, total_read {}",
        //              _buffer->total_written(),
        //              total_read());


        // Maybe adjust read pointers from min read index?
        // This would mean that *all* readers must be > (passed) the write index
        if (items_avail < items_required && _buffer->write_index() < read_index()) {
            // GR_LOG_DEBUG(d_debug_logger, "Calling adjust_buffer_data ");

            switch (_buffer_cuda_sm->type()) {
            case buffer_cuda_sm_type::H2D:
            case buffer_cuda_sm_type::D2D:
                return _buffer_sm->adjust_buffer_data(buffer_cuda_sm::cuda_memcpy,
                                                      buffer_cuda_sm::cuda_memmove);
            case buffer_cuda_sm_type::D2H:
                return _buffer_sm->adjust_buffer_data(std::memcpy, std::memmove);
            default:
                return false;
            }
        }

        return false;
    }
};

class buffer_cuda_sm_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_cuda_sm_properties(buffer_cuda_sm_type buffer_type_)
        : buffer_properties(), _buffer_type(buffer_type_)
    {
        _bff = buffer_cuda_sm::make;
    }
    buffer_cuda_sm_type buffer_type() { return _buffer_type; }
    static std::shared_ptr<buffer_properties>
    make(buffer_cuda_sm_type buffer_type_ = buffer_cuda_sm_type::D2D)
    {
        return std::static_pointer_cast<buffer_properties>(
            std::make_shared<buffer_cuda_sm_properties>(buffer_type_));
    }

private:
    buffer_cuda_sm_type _buffer_type;
};


} // namespace gr

#define CUDA_BUFFER_SM_ARGS_H2D buffer_cuda_sm_properties::make(buffer_cuda_sm_type::H2D)
#define CUDA_BUFFER_SM_ARGS_D2H buffer_cuda_sm_properties::make(buffer_cuda_sm_type::D2H)
#define CUDA_BUFFER_SM_ARGS_D2D buffer_cuda_sm_properties::make(buffer_cuda_sm_type::D2D)
