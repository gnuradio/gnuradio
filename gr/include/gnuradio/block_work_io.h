#pragma once

#include <gnuradio/buffer.h>
#include <gnuradio/port.h>
#include <gnuradio/tag.h>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace gr {

/**
 * @brief Struct for passing all information needed for input data to block::work
 *
 */
class block_work_input
{

private:
    buffer_reader* _buffer;

public:
    size_t n_items = 0;
    size_t n_consumed =
        0; // output the number of items that were consumed on the work() call
    port_ptr port = nullptr;
    block_work_input(int n_items_, buffer_reader* p_buf_, port_ptr p = nullptr)
        : _buffer(p_buf_), n_items(n_items_), port(p)
    {
    }

    buffer_reader& buf() { return *_buffer; }
    buffer_reader* bufp() { return _buffer; }
    void reset()
    {
        n_items = 0;
        n_consumed = 0;
    }
    template <typename T>
    const T* items() const
    {
        return static_cast<const T*>(_buffer->read_ptr());
    }
    const void* raw_items() const { return _buffer->read_ptr(); }

    uint64_t nitems_read() { return _buffer->total_read(); }

    void consume(int num) { n_consumed = num; }

    std::vector<tag_t> tags_in_window(const uint64_t item_start, const uint64_t item_end)
    {
        return _buffer->tags_in_window(item_start, item_end);
    }
};

/**
 * @brief Struct for passing all information needed for output data from block::work
 *
 */
class block_work_output
{
private:
    buffer* _buffer;

public:
    size_t n_items;

    size_t n_produced =
        0; // output the number of items that were produced on the work() call
    port_ptr port = nullptr;
    block_work_output(int _n_items, buffer* p_buf_, port_ptr p = nullptr)
        : _buffer(p_buf_), n_items(_n_items), port(p)

    {
    }
    buffer& buf() { return *_buffer; }
    buffer* bufp() { return _buffer; }
    void reset()
    {
        n_items = 0;
        n_produced = 0;
    }
    template <typename T>
    T* items() const
    {
        return static_cast<T*>(_buffer->write_ptr());
    }
    void* raw_items() const { return _buffer->write_ptr(); }

    uint64_t nitems_written() { return _buffer->total_written(); }
    void produce(int num) { n_produced = num; }

    void add_tag(tag_t& tag) { _buffer->add_tag(tag); }
    void add_tag(uint64_t offset, tag_map map) { _buffer->add_tag(offset, map); }
    void add_tag(uint64_t offset, pmtf::map map) { _buffer->add_tag(offset, map); }
};

/**
 * @brief Enum for return codes from calls to block::work
 *
 */
enum class work_return_code_t {
    WORK_ERROR = -100, /// error occurred in the work function
    WORK_INSUFFICIENT_OUTPUT_ITEMS =
        -3, /// work requires a larger output buffer to produce output
    WORK_INSUFFICIENT_INPUT_ITEMS =
        -2, /// work requires a larger input buffer to produce output
    WORK_DONE =
        -1, /// this block has completed its processing and the flowgraph should be done
    WORK_OK = 0, /// work call was successful and return values in i/o structs are valid
    WORK_CALLBACK_INITIATED =
        1, /// rather than blocking in the work function, the block will call back to the
           /// parent interface when it is ready to be called again
};
template <typename T>
class io_vec_wrap
{
private:
    std::vector<T> _vec;
    std::vector<std::string> _names;

public:
    T& operator[](size_t idx) { return _vec[idx]; }
    T& operator[](const std::string& name)
    {
        auto it = std::find(std::begin(_names), std::end(_names), name);
        if (it != std::end(_names)) {
            return _vec[it - _names.begin()];
        }

        throw std::runtime_error(fmt::format("Named io entry {} not found", name));
    }
    auto begin() noexcept { return _vec.begin(); }
    auto end() noexcept { return _vec.end(); }
    auto back() noexcept { return _vec.back(); }
    auto size() const noexcept { return _vec.size(); }
    auto empty() const noexcept { return _vec.empty(); }
    auto clear() noexcept
    {
        _vec.clear();
        _names.clear();
    }
    void append_item(const T& element, const std::string& name)
    {
        _vec.push_back(element);
        _names.push_back(name);
    }
};

class work_io
{
public:
    work_io() {}
    work_io(const work_io&) = delete;
    work_io& operator=(const work_io&) = delete;

    io_vec_wrap<block_work_input>& inputs() { return _inputs; }
    io_vec_wrap<block_work_output>& outputs() { return _outputs; }
    void clear()
    {
        _inputs.clear();
        _outputs.clear();
    }
    void reset()
    {
        for (auto& w : _inputs) {
            w.reset();
        }
        for (auto& w : _outputs) {
            w.reset();
        }
    }
    // Convenience Methods
    void consume_each(size_t n_items)
    {
        for (auto& w : inputs()) {
            w.n_consumed = n_items;
        }
    }
    void produce_each(size_t n_items)
    {
        for (auto& w : outputs()) {
            w.n_produced = n_items;
        }
    }
    size_t min_noutput_items()
    {
        auto result = (std::min_element(
            _outputs.begin(),
            _outputs.end(),
            [](const block_work_output& lhs, const block_work_output& rhs) {
                return (lhs.n_items < rhs.n_items);
            }));
        return result->n_items;
    }

    size_t min_ninput_items()
    {
        auto result = (std::min_element(
            _inputs.begin(),
            _inputs.end(),
            [](const block_work_input& lhs, const block_work_input& rhs) {
                return (lhs.n_items < rhs.n_items);
            }));
        return result->n_items;
    }

    std::vector<const void*> all_input_ptrs()
    {
        std::vector<const void*> ret(inputs().size());
        for (size_t idx = 0; idx < inputs().size(); idx++) {
            ret[idx] = inputs()[idx].buf().read_ptr();
        }

        return ret;
    }
    std::vector<void*> all_output_ptrs()
    {
        std::vector<void*> ret(outputs().size());
        for (size_t idx = 0; idx < outputs().size(); idx++) {
            ret[idx] = outputs()[idx].buf().write_ptr();
        }

        return ret;
    }

private:
    friend class block;
    io_vec_wrap<block_work_input> _inputs;
    io_vec_wrap<block_work_output> _outputs;

    void add_input(port_ptr p)
    {
        _inputs.append_item(block_work_input(0, p->get_buffer_reader(), p), p->name());
    }

    void add_output(port_ptr p)
    {
        _outputs.append_item(block_work_output(0, p->get_buffer(), p), p->name());
    }
};

} // namespace gr
