#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include <gnuradio/buffer_sm.h>

namespace gr {
enum class buffer_cpu_host_type { D2D, H2D, D2H, UNKNOWN };
class buffer_cpu_host_reader;
class buffer_cpu_host : public buffer_sm
{
private:
    uint8_t* _host_buffer;
    uint8_t* _device_buffer;
    buffer_cpu_host_type _transfer_type = buffer_cpu_host_type::UNKNOWN;

public:
    buffer_cpu_host(size_t num_items,
                    size_t item_size,
                    buffer_cpu_host_type type,
                    std::shared_ptr<buffer_properties> buf_properties);

    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    void* read_ptr(size_t index) override;
    void* write_ptr() override;

    void post_write(int num_items) override;

    buffer_cpu_host_type transfer_type() { return _transfer_type; }

    buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                  size_t itemsize) override;

    virtual bool output_blocked_callback(bool force = false) override
    {
        switch (_transfer_type) {
        case buffer_cpu_host_type::H2D:
            return output_blocked_callback_logic(force, std::memmove);
        case buffer_cpu_host_type::D2D:
        case buffer_cpu_host_type::D2H:
            return output_blocked_callback_logic(force, std::memmove);
        default:
            return false;
        }
    }
};

class buffer_cpu_host_reader : public buffer_sm_reader
{
private:
    buffer_cpu_host* _buffer_cpu_host;

public:
    buffer_cpu_host_reader(buffer_cpu_host* bufp,
                           std::shared_ptr<buffer_properties> buf_props,
                           size_t itemsize,
                           size_t read_index = 0)
        : buffer_sm_reader(bufp, itemsize, buf_props, read_index), _buffer_cpu_host(bufp)
    {
    }

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

            switch (_buffer_cpu_host->transfer_type()) {
            case buffer_cpu_host_type::H2D:
            case buffer_cpu_host_type::D2D:
                return _buffer_cpu_host->adjust_buffer_data(std::memcpy, std::memmove);
            case buffer_cpu_host_type::D2H:
                return _buffer_cpu_host->adjust_buffer_data(std::memcpy, std::memmove);
            default:
                return false;
            }
        }

        return false;
    }
};

class buffer_cpu_host_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_cpu_host_properties(buffer_cpu_host_type buffer_type_)
        : buffer_properties(), _buffer_type(buffer_type_)
    {
        _bff = buffer_cpu_host::make;
    }
    buffer_cpu_host_type buffer_type() { return _buffer_type; }
    static std::shared_ptr<buffer_properties>
    make(buffer_cpu_host_type buffer_type_ = buffer_cpu_host_type::D2D)
    {
        return std::static_pointer_cast<buffer_properties>(
            std::make_shared<buffer_cpu_host_properties>(buffer_type_));
    }

private:
    buffer_cpu_host_type _buffer_type;
};


#define BUFFER_CPU_HOST_ARGS_H2D \
    buffer_cpu_host_properties::make(buffer_cpu_host_type::H2D)
#define BUFFER_CPU_HOST_ARGS_D2H \
    buffer_cpu_host_properties::make(buffer_cpu_host_type::D2H)
#define BUFFER_CPU_HOST_ARGS_D2D \
    buffer_cpu_host_properties::make(buffer_cpu_host_type::D2D)


} // namespace gr
