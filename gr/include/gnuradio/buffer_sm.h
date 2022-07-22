#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include <gnuradio/buffer.h>
// #include <gnuradio/logger.h>

namespace gr {

typedef void* (*memcpy_func_t)(void* dest, const void* src, std::size_t count);
typedef void* (*memmove_func_t)(void* dest, const void* src, std::size_t count);

class buffer_sm_reader;
class buffer_sm : public buffer
{
private:
    std::vector<uint8_t> _buffer;

    // logger_ptr d_logger;
    // logger_ptr d_debug_logger;

public:
    using sptr = std::shared_ptr<buffer_sm>;
    buffer_sm(size_t num_items,
              size_t item_size,
              std::shared_ptr<buffer_properties> buf_properties);

    static buffer_uptr
    make(size_t num_items,
         size_t item_size,
         std::shared_ptr<buffer_properties> buffer_properties = nullptr);

    void* read_ptr(size_t index) override;
    void* write_ptr() override;

    void post_write(int num_items) override;

    virtual bool
    output_blocked_callback_logic(bool force = false,
                                  memmove_func_t memmove_func = std::memmove);

    bool output_blocked_callback(bool force = false) override;
    size_t space_available() override;

    bool write_info(buffer_info_t& info) override;
    buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                  size_t itemsize) override;

    bool adjust_buffer_data(memcpy_func_t memcpy_func, memmove_func_t memmove_func);
};

class buffer_sm_reader : public buffer_reader
{
private:
    logger_ptr d_logger;
    logger_ptr d_debug_logger;

protected:
    buffer_sm* _buffer_sm;

public:
    buffer_sm_reader(buffer_sm* buffer,
                     size_t itemsize,
                     std::shared_ptr<buffer_properties> buf_props = nullptr,
                     size_t read_index = 0);

    void post_read(int num_items) override;

    bool input_blocked_callback(size_t items_required) override;
    size_t bytes_available() override;
};


class buffer_sm_properties : public buffer_properties
{
public:
    buffer_sm_properties();

    static std::shared_ptr<buffer_properties> make();
};


#define SM_BUFFER_ARGS buffer_sm_properties::make()

} // namespace gr
