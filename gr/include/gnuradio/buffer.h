#pragma once

#include <gnuradio/api.h>
#include <gnuradio/custom_lock.h>
#include <gnuradio/logger.h>
#include <gnuradio/neighbor_interface.h>
#include <gnuradio/tag.h>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace gr {

/**
 * @brief Information about the current state of the buffer
 *
 * The buffer_info_t class is used to return information about the current state of the
 * buffer for reading or writing, as in how many items are contained, or how much space is
 * there to write into, as well as the total items read/written
 *
 */
struct GR_RUNTIME_API buffer_info_t {
    void* ptr;
    int n_items; // number of items available to be read or written
    size_t item_size;
    int total_items; // the total number of items read/written from/to this buffer
};

class buffer_reader;
using buffer_reader_uptr = std::unique_ptr<buffer_reader>;

class buffer;
class buffer_properties;
using buffer_factory_function = std::function<std::unique_ptr<buffer>(
    size_t, size_t, std::shared_ptr<buffer_properties>)>;

using buffer_reader_factory_function = std::function<std::unique_ptr<buffer_reader>(
    size_t, std::shared_ptr<buffer_properties>)>;

/**
 * @brief Base class for passing custom buffer properties into factory method
 *
 * Buffer Properties will vary according to the particular buffer
 */
class GR_RUNTIME_API buffer_properties
    : public std::enable_shared_from_this<buffer_properties>
{
public:
    buffer_properties() {}
    virtual ~buffer_properties() {}

    size_t buffer_size() { return _buffer_size; }
    size_t max_buffer_size() { return _max_buffer_size; }
    size_t min_buffer_size() { return _min_buffer_size; }
    size_t max_buffer_fill() { return _max_buffer_fill; }
    size_t min_buffer_fill() { return _min_buffer_fill; }
    size_t max_buffer_read() { return _max_buffer_read; }
    size_t min_buffer_read() { return _min_buffer_read; }

    auto set_buffer_size(size_t buffer_size)
    {
        _buffer_size = buffer_size;
        return shared_from_this();
    }
    auto set_max_buffer_size(size_t max_buffer_size)
    {
        _max_buffer_size = max_buffer_size;
        return shared_from_this();
    }
    auto set_min_buffer_size(size_t min_buffer_size)
    {
        _min_buffer_size = min_buffer_size;
        return shared_from_this();
    }
    auto set_max_buffer_fill(size_t max_buffer_fill)
    {
        _max_buffer_fill = max_buffer_fill;
        return shared_from_this();
    }
    auto set_min_buffer_fill(size_t min_buffer_fill)
    {
        _min_buffer_fill = min_buffer_fill;
        return shared_from_this();
    }
    auto set_max_buffer_read(size_t max_buffer_read)
    {
        _max_buffer_read = max_buffer_read;
        return shared_from_this();
    }
    auto set_min_buffer_read(size_t min_buffer_read)
    {
        _min_buffer_read = min_buffer_read;
        return shared_from_this();
    }
    buffer_factory_function factory() { return _bff; }
    buffer_reader_factory_function reader_factory() { return _brff; }

    auto independent_reader() { return _independent_reader; }

    virtual std::string to_json() { return "{ }"; }

protected:
    size_t _buffer_size = 0;
    size_t _max_buffer_size = 0;
    size_t _min_buffer_size = 0;
    size_t _max_buffer_fill = 0;
    size_t _min_buffer_fill = 0;
    size_t _max_buffer_read = 0;
    size_t _min_buffer_read = 0;

    buffer_factory_function _bff = nullptr;
    buffer_reader_factory_function _brff = nullptr;

    bool _independent_reader = false;
};

/**
 * @brief Abstract buffer class
 *
 */
class GR_RUNTIME_API buffer : public custom_lock_if
{
protected:
    std::string _name;
    std::string _type;

    size_t _write_index = 0;
    size_t _num_items;
    size_t _item_size;
    size_t _buf_size;

    uint64_t _total_written = 0;

    std::shared_ptr<buffer_properties> _buf_properties;

    void set_type(const std::string& type) { _type = type; }

    std::mutex _buf_mutex;
    std::vector<tag_t> _tags;

    std::vector<buffer_reader*> _readers;

    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    std::condition_variable d_cv;
    bool d_callback_flag = false;
    uint32_t d_active_pointer_counter = 0;

public:
    buffer(size_t num_items,
           size_t item_size,
           std::shared_ptr<buffer_properties> buf_properties)
        : _num_items(num_items),
          _item_size(item_size),
          _buf_size(num_items * item_size),
          _buf_properties(buf_properties)
    {
    }
    virtual ~buffer() {}
    size_t item_size() { return _item_size; }
    size_t num_items() { return _num_items; }
    size_t buf_size() { return _buf_size; }
    size_t write_index() { return _write_index; }
    uint64_t total_written() const { return _total_written; }
    const std::vector<tag_t>& tags() { return _tags; }
    std::mutex* mutex() { return &_buf_mutex; }

    // std::shared_ptr<buffer_properties>& buf_properties() { return _buf_properties; }
    size_t max_buffer_size()
    {
        return _buf_properties ? _buf_properties->max_buffer_size() : 0;
    }
    size_t min_buffer_size()
    {
        return _buf_properties ? _buf_properties->min_buffer_size() : 0;
    }
    size_t max_buffer_fill()
    {
        return _buf_properties ? _buf_properties->max_buffer_fill() : 0;
    }
    size_t min_buffer_fill()
    {
        return _buf_properties ? _buf_properties->min_buffer_fill() : 0;
    }

    std::vector<buffer_reader*>& readers() { return _readers; }


    /**
     * @brief Return the pointer into the buffer at the given index
     *
     * @param index
     * @return void*
     */
    virtual void* read_ptr(size_t index) = 0;

    /**
     * @brief Return the write pointer into the beginning of the buffer
     *
     * @return void*
     */
    virtual void* write_ptr() = 0;

    /**
     * @brief Return current buffer state for writing
     *
     * @param info Reference to \buffer_info_t struct
     * @return true if info is valid
     * @return false if info is not valid (e.g. could not acquire mutex)
     */
    virtual bool write_info(buffer_info_t& info);
    virtual size_t space_available();


    /**
     * @brief Add Tags onto the tag queue
     *
     * @param num_items
     * @param tags
     */
    void add_tags(size_t num_items, std::vector<tag_t>& tags);

    const std::vector<tag_t>& tags() const { return _tags; }

    void add_tag(tag_t tag);
    void add_tag(uint64_t offset, tag_map map);
    void add_tag(uint64_t offset, pmtf::map map);

    void propagate_tags(buffer_reader* p_in_buf, int n_consumed);

    void prune_tags();

    /**
     * @brief Updates the write pointers of the buffer
     *
     * @param num_items Number of items that were written to the buffer
     */
    virtual void post_write(int num_items) = 0;

    /**
     * @brief Set the name of the buffer
     *
     * @param name
     */
    void set_name(const std::string& name) { _name = name; }

    /**
     * @brief Get the name of the buffer
     *
     * @return std::string
     */
    std::string name() { return _name; }

    /**
     * @brief Get the type of the buffer
     *
     * @return std::string
     */
    std::string type() { return _type; }

    /**
     * @brief Create a reader object and reference to this buffer
     *
     * @param buf_props
     * @param itemsize itemsize in bytes on the destination side
     * @return buffer_reader_uptr
     */
    virtual buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                          size_t itemsize) = 0;
    // void drop_reader(buffer_reader_uptr);


    /*!
     * \brief Returns true if the current thread is ready to execute
     * output_blocked_callback(), false otherwise. Note if the default
     * output_blocked_callback is overridden this function should also be
     * overridden.
     */
    virtual bool output_blkd_cb_ready([[maybe_unused]] int output_multiple)
    {
        return false;
    }

    virtual bool output_blocked_callback(bool force = false)
    {
        // Only singly mapped buffers need to do anything with this callback
        return false;
    }

    /*!
     * \brief Increment the number of active pointers for this buffer.
     */
    inline void increment_active()
    {
        std::unique_lock<std::mutex> lock(_buf_mutex);

        d_cv.wait(lock, [this]() { return d_callback_flag == false; });
        ++d_active_pointer_counter;
    }

    /*!
     * \brief Decrement the number of active pointers for this buffer and signal
     * anyone waiting when the count reaches zero.
     */
    inline void decrement_active()
    {
        std::unique_lock<std::mutex> lock(_buf_mutex);

        if (--d_active_pointer_counter == 0)
            d_cv.notify_all();
    }


    /*!
     * \brief "on_lock" function from the custom_lock_if.
     */
    void on_lock(std::unique_lock<std::mutex>& lock) override;

    /*!
     * \brief "on_unlock" function from the custom_lock_if.
     */
    void on_unlock() override;

    friend std::ostream& operator<<(std::ostream& os, const buffer& buf);
};

using buffer_uptr = std::unique_ptr<buffer>;


class GR_RUNTIME_API buffer_reader
{
protected:
    buffer* _buffer; // the buffer that owns this reader
    std::shared_ptr<buffer_properties> _buf_properties;
    uint64_t _total_read = 0;
    size_t _itemsize;
    size_t _read_index = 0;
    std::mutex _rdr_mutex;


public:
    buffer_reader(buffer* buffer,
                  std::shared_ptr<buffer_properties> buf_props,
                  size_t itemsize,
                  size_t read_index = 0)
        : _buffer(buffer),
          _buf_properties(buf_props),
          _itemsize(itemsize),
          _read_index(read_index)
    {
    }
    virtual ~buffer_reader() {}
    size_t read_index() { return _read_index; }
    void set_read_index(size_t r) { _read_index = r; }
    virtual void* read_ptr() { return _buffer->read_ptr(_read_index); }
    virtual void post_read(int num_items) = 0;
    uint64_t total_read() const { return _total_read; }
    // std::shared_ptr<buffer_properties>& buf_properties() { return _buf_properties; }
    size_t max_buffer_read()
    {
        return _buf_properties ? _buf_properties->max_buffer_read() : 0;
    }
    size_t min_buffer_read()
    {
        return _buf_properties ? _buf_properties->min_buffer_read() : 0;
    }
    size_t item_size() { return _itemsize; }
    size_t buffer_item_size() { return _buffer->item_size(); }

    std::mutex* mutex() { return &_rdr_mutex; }

    /**
     * @brief Return the number of items available to be read
     *
     * @return uint64_t
     */
    virtual uint64_t items_available();

    /**
     * @brief Return the number of bytes available to be read
     *
     * @return uint64_t
     */
    virtual uint64_t bytes_available();

    /**
     * @brief Return current buffer state for reading
     *
     * @param info Reference to \buffer_info_t struct
     * @return true if info is valid
     * @return false if info is not valid (e.g. could not acquire mutex)
     */
    virtual bool read_info(buffer_info_t& info);

    /*!
     * \brief Returns true when the current thread is ready to call the callback,
     * false otherwise. Note if input_blocked_callback is overridden then this
     * function should also be overridden.
     */
    virtual bool input_blkd_cb_ready([[maybe_unused]] int items_required,
                                     [[maybe_unused]] unsigned read_index)
    {
        return false;
    }

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the input is blocked. Delegate calls to buffer class's
     * input_blocked_callback(). Override this function if needed.
     */
    virtual bool input_blocked_callback(size_t items_required)
    {
        // Only singly mapped buffers need to do anything with this callback
        return false;
    }

    std::vector<tag_t> tags_in_window(const uint64_t item_start, const uint64_t item_end);

    /**
     * @brief Return the tags associated with this buffer
     *
     * @param num_items Number of items that will be associated with the work call, and
     * thus return the tags from the current read pointer to this specified number of
     * items
     * @return std::vector<tag_t> Returns the vector of tags
     */
    virtual std::vector<tag_t> get_tags(size_t num_items);

    virtual const std::vector<tag_t>& tags() const;

    void set_parent_intf(neighbor_interface_sptr sched) { p_scheduler = sched; }
    void notify_scheduler();
    void notify_scheduler_input();
    void notify_scheduler_output();

    inline void increment_active() { _buffer->increment_active(); }
    inline void decrement_active() { _buffer->decrement_active(); }

protected:
    neighbor_interface_sptr p_scheduler = nullptr;
};

} // namespace gr
