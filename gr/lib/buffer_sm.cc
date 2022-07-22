#include <gnuradio/buffer_sm.h>

#include <gnuradio/logger.h>

namespace gr {

buffer_reader_uptr buffer_sm::add_reader(std::shared_ptr<buffer_properties> buf_props,
                                         size_t itemsize)
{
    auto r = std::make_unique<buffer_sm_reader>(this, itemsize, buf_props, _write_index);
    _readers.push_back(r.get());
    return r;
}

buffer_sm::buffer_sm(size_t num_items,
                     size_t item_size,
                     std::shared_ptr<buffer_properties> buf_properties)
    : buffer(num_items, item_size, buf_properties)
{
    _buffer.resize(_buf_size); // singly mapped buffer
    _write_index = 0;

    set_type("buffer_sm");


    gr::configure_default_loggers(d_logger, d_debug_logger, _type);
}

buffer_uptr buffer_sm::make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties)
{
    return buffer_uptr(new buffer_sm(num_items, item_size, buffer_properties));
}

void* buffer_sm::read_ptr(size_t index) { return (void*)&_buffer[index]; }
void* buffer_sm::write_ptr() { return (void*)&_buffer[_write_index]; }

void buffer_sm::post_write(int num_items)
{
    std::scoped_lock guard(_buf_mutex);

    size_t bytes_written = num_items * _item_size;
    // num_items were written to the buffer

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

bool buffer_sm::output_blocked_callback_logic(bool force, memmove_func_t memmove_func)
{
    auto space_avail = space_available();

    // if (((space_avail > 0) && ((space_avail / output_multiple) * output_multiple ==
    // 0)) ||
    if ((space_avail > 0) || force) {
        // Find reader with the smallest read index
        uint32_t min_read_idx = _readers[0]->read_index();
        for (size_t idx = 1; idx < _readers.size(); ++idx) {
            // Record index of reader with minimum read-index
            if (_readers[idx]->read_index() < min_read_idx) {
                min_read_idx = _readers[idx]->read_index();
            }
        }

        // GR_LOG_DEBUG(d_debug_logger,
        //              "output_blocked_callback, space_avail {}, min_read_idx {}, "
        //              "_write_index {}",
        //              space_avail,
        //              min_read_idx,
        //              _write_index);

        // Make sure we have enough room to start writing back at the beginning
        if ((min_read_idx == 0) || (min_read_idx >= _write_index)) {
            return false;
        }

        // Determine how much "to be read" data needs to be moved
        auto to_move_bytes = _write_index - min_read_idx;

        if (to_move_bytes > min_read_idx) {
            return false;
        }

        // GR_LOG_DEBUG(
        //     d_debug_logger, "output_blocked_callback, moving {} bytes", to_move_bytes);

        // Shift "to be read" data back to the beginning of the buffer
        std::memmove(_buffer.data(), _buffer.data() + (min_read_idx), to_move_bytes);

        // Adjust write index and each reader index
        _write_index -= min_read_idx;

        // for (size_t idx = 0; idx < _readers.size(); ++idx) {
        //     GR_LOG_DEBUG(d_debug_logger,
        //                  "output_blocked_callback,setting _read_index to {}",
        //                  _readers[idx]->read_index() - min_read_idx);
        //     _readers[idx]->set_read_index(_readers[idx]->read_index() - min_read_idx);
        // }

        return true;
    }

    return false;
}

bool buffer_sm::output_blocked_callback(bool force)
{
    std::scoped_lock guard(_buf_mutex);

    return output_blocked_callback_logic(force, std::memmove);
}

size_t buffer_sm::space_available()
{
    // Find the max number of items available across readers

    uint64_t min_items_read = std::numeric_limits<uint64_t>::max();
    size_t min_read_idx = 0;
    for (size_t idx = 0; idx < _readers.size(); idx++) {
        std::scoped_lock lck{ *(_readers[idx]->mutex()) };
        auto total_read = _readers[idx]->total_read();
        if (total_read < min_items_read) {
            min_items_read = total_read;
            min_read_idx = _readers[idx]->read_index();
        }
    }

    size_t space = (_buf_size - _write_index) / _item_size;

    if (min_read_idx == _write_index) {
        // If the (min) read index and write index are equal then the buffer
        // is either completely empty or completely full depending on if
        // the number of items read matches the number written
        if (min_items_read != total_written()) {
            space = 0;
        }
    }
    else if (min_read_idx > _write_index) {
        space = (min_read_idx - _write_index) / _item_size;
    }

    if (space == 0)
        return space;
    // Only half fill the buffer
    // Leave extra space in case the reader gets stuck and needs realignment

    space = std::min(space, _num_items / 2);

    return space;
}

bool buffer_sm::write_info(buffer_info_t& info)
{
    std::scoped_lock guard(_buf_mutex);

    info.ptr = write_ptr();
    info.n_items = space_available();
    if (info.n_items < 0)
        info.n_items = 0;
    info.item_size = _item_size;
    info.total_items = _total_written;

    return true;
}

bool buffer_sm::adjust_buffer_data(memcpy_func_t memcpy_func, memmove_func_t memmove_func)
{

    // Find reader with the smallest read index that is greater than the
    // write index
    // auto min_reader_index = std::numeric_limits<size_t>::max();
    auto min_read_idx = std::numeric_limits<size_t>::max();
    for (size_t idx = 0; idx < _readers.size(); ++idx) {
        if (_readers[idx]->read_index() > write_index()) {
            // Record index of reader with minimum read-index
            // FIXME: What if one of the readers has wrapped back around?
            //  -- in that case this should use items_available() callback
            if (_readers[idx]->read_index() < min_read_idx) {
                min_read_idx = _readers[idx]->read_index();
                // min_reader_index = idx;
            }
        }
    }

    // Note items_avail might be zero, that's okay.
    auto max_bytes_avail = _buf_size - min_read_idx;
    auto max_items_avail = max_bytes_avail / _item_size;
    auto gap = min_read_idx - _write_index;
    if (_write_index > min_read_idx || max_bytes_avail > gap) {
        return false;
    }

    // GR_LOG_DEBUG(d_debug_logger,
    //              "adust_buffer_data: max_bytes_avail {}, gap {}",
    //              max_bytes_avail,
    //              gap);


    // Shift existing data down to make room for blocked data at end of buffer
    auto move_data_size = _write_index;
    auto dest = _buffer.data() + max_bytes_avail;
    memmove_func(dest, _buffer.data(), move_data_size);

    // Next copy the data from the end of the buffer back to the beginning
    auto avail_data_size = max_bytes_avail;
    auto src = _buffer.data() + (min_read_idx * _item_size);
    memcpy_func(_buffer.data(), src, avail_data_size);

    // Finally adjust all reader pointers
    for (size_t idx = 0; idx < _readers.size(); ++idx) {
        // GR_LOG_DEBUG(d_debug_logger,
        //              "adjust_buffer_data,setting _read_index to {}",
        //              _readers[idx]->read_index() - min_read_idx);
        _readers[idx]->set_read_index(max_items_avail - _readers[idx]->items_available());
    }

    // Now adjust write pointer
    _write_index += max_items_avail;

    return true;
}


buffer_sm_reader::buffer_sm_reader(buffer_sm* bufp,
                                   size_t itemsize,
                                   std::shared_ptr<buffer_properties> buf_props,
                                   size_t read_index)
    : buffer_reader(bufp, buf_props, itemsize, read_index), _buffer_sm(bufp)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_sm_reader");
}

void buffer_sm_reader::post_read(int num_items)
{
    std::scoped_lock guard(_rdr_mutex);

    // GR_LOG_DEBUG(
    // d_debug_logger, "post_read: _read_index {}, num_items {}", _read_index, num_items);

    // advance the read pointer
    _read_index += num_items * _itemsize; //_buffer->item_size();
    _total_read += num_items;
    if (_read_index == _buffer->buf_size()) {
        _read_index = 0;
    }
    if (_read_index > _buffer->buf_size()) {
        // GR_LOG_INFO(d_logger,
        //             "too far: num_items {}, prev_index {}, post_index {}",
        //             num_items,
        //             _read_index - num_items * _buffer->item_size(),
        //             _read_index);

        // // throw std::runtime_error("buffer_sm_reader: Wrote too far into buffer");
    }

    // GR_LOG_DEBUG(d_debug_logger, "post_read: _read_index {}", _read_index);
}

bool buffer_sm_reader::input_blocked_callback(size_t items_required)
{
    // Only singly mapped buffers need to do anything with this callback
    std::scoped_lock guard(*(_buffer->mutex()));

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
        return _buffer_sm->adjust_buffer_data(std::memcpy, std::memmove);
    }

    return false;
}

size_t buffer_sm_reader::bytes_available()
{
    // Can only read up to to the write_index, or the end of the buffer
    // there is no wraparound

    size_t ret = 0;

    size_t w = _buffer->write_index();
    size_t r = _read_index;

    if (w < r) {
        ret = (_buffer->buf_size() - r);
    }
    else if (w == r && total_read() < _buffer->total_written()) {
        ret = (_buffer->buf_size() - r);
    }
    else {
        ret = (w - r);
    }

    // return ret;

    // GR_LOG_DEBUG(d_debug_logger,
    //              "items_available: write_index {}, read_index {}, ret {}, total_read "
    //              "{}, total_written {}",
    //              w,
    //              r,
    //              ret,
    //              total_read(),
    //              _buffer->total_written());

    if (_buffer->total_written() - total_read() < ret * _itemsize) {
        // GR_LOG_DEBUG(d_debug_logger,
        //              "check_math {} {} {} {}",
        //              _buffer->total_written() - total_read(),
        //              ret,
        //              total_read(),
        //              _buffer->total_written());
    }

    return ret; // in bytes
}


buffer_sm_properties::buffer_sm_properties() : buffer_properties()
{
    _bff = buffer_sm::make;
}

std::shared_ptr<buffer_properties> buffer_sm_properties::make()
{
    return std::static_pointer_cast<buffer_properties>(
        std::make_shared<buffer_sm_properties>());
}

} // namespace gr
