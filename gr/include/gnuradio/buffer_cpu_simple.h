#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include <gnuradio/buffer.h>

namespace gr {

class buffer_cpu_simple_reader;
class buffer_cpu_simple : public buffer
{
private:
    std::vector<uint8_t> _buffer;

public:
    buffer_cpu_simple(size_t num_items,
                      size_t item_size,
                      std::shared_ptr<buffer_properties> buf_properties);

    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    void* read_ptr(size_t index) override;
    void* write_ptr() override;

    void post_write(int num_items) override;

    buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                  size_t itemsize) override;
};

class buffer_cpu_simple_reader : public buffer_reader
{
public:
    buffer_cpu_simple_reader(buffer* bufp,
                             std::shared_ptr<buffer_properties> buf_props,
                             size_t itemsize,
                             size_t read_index = 0)
        : buffer_reader(bufp, buf_props, itemsize, read_index)
    {
    }

    void post_read(int num_items) override;
};

class buffer_cpu_simple_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_cpu_simple_properties() : buffer_properties()
    {
        _bff = buffer_cpu_simple::make;
    }
    static std::shared_ptr<buffer_properties> make()
    {
        return std::static_pointer_cast<buffer_properties>(
            std::make_shared<buffer_cpu_simple_properties>());
    }
};

} // namespace gr
