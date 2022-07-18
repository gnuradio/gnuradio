#include <gnuradio/buffer.h>

namespace gr {

size_t buffer::space_available()
{
    // Find the max number of bytes available across readers
    uint64_t n_available = 0;
    for (auto& r : _readers) {
        auto n = r->bytes_available();
        if (n > n_available) {
            n_available = n;
        }
    }

    int space_in_items = (_num_items * _item_size - n_available) / _item_size - 1;

    if (space_in_items < 0)
        space_in_items = 0;
    space_in_items =
        std::min(space_in_items, (int)(_num_items / 2)); // move to a max_fill parameter

    return space_in_items;
}
bool buffer::write_info(buffer_info_t& info)
{
    std::scoped_lock guard(_buf_mutex);


    auto space = space_available();
    info.ptr = write_ptr();
    info.n_items = space;
    info.item_size = _item_size;
    info.total_items = _total_written;

    return true;
}
void buffer::add_tags(size_t num_items, std::vector<tag_t>& tags)
{
    std::scoped_lock guard(_buf_mutex);

    for (auto tag : tags) {
        if (tag.offset() < _total_written - num_items || tag.offset() >= _total_written) {
        }
        else {
            _tags.push_back(tag);
        }
    }
}


void buffer::add_tag(tag_t tag)
{
    std::scoped_lock guard(_buf_mutex);
    _tags.push_back(tag);
}
void buffer::add_tag(uint64_t offset, tag_map map)
{
    std::scoped_lock guard(_buf_mutex);
    _tags.emplace_back(offset, map);
}

void buffer::add_tag(uint64_t offset, pmtf::map map)
{
    std::scoped_lock guard(_buf_mutex);
    _tags.emplace_back(offset, map);
}

void buffer::propagate_tags(buffer_reader* p_in_buf, int n_consumed)
{
    double relative_rate =
        (double)p_in_buf->item_size() / (double)p_in_buf->buffer_item_size();
    std::scoped_lock guard(_buf_mutex);
    for (auto& t : p_in_buf->tags()) {
        uint64_t new_offset = t.offset();
        if (relative_rate != 1.0) {
            new_offset = t.offset() / relative_rate;
        }
        // Propagate the tags that occurred in the processed window
        if (new_offset >= total_written() && new_offset < total_written() + n_consumed) {
            // std::cout << "adding tag" << std::endl;
            _tags.push_back(t);

            if (relative_rate != 1.0) {
                _tags[_tags.size() - 1].set_offset(new_offset);
            }
        }
    }
}

void buffer::prune_tags()
{
    std::scoped_lock guard(_buf_mutex);

    // Find the min number of items available across readers
    auto n_read = total_written();
    for (auto& r : _readers) {
        auto n = r->total_read();
        if (n < n_read) {
            n_read = n;
        }
    }

    auto t = std::begin(_tags);
    while (t != std::end(_tags)) {
        // Do some stuff
        if (t->offset() < n_read) {
            t = _tags.erase(t);
            // std::cout << "removing tag" << std::endl;
        }
        else {
            ++t;
        }
    }
}

size_t buffer_reader::items_available() { return bytes_available() / _itemsize; }

size_t buffer_reader::bytes_available()
{
    size_t w = _buffer->write_index();
    size_t r = _read_index;

    if (w < r)
        w += _buffer->buf_size();
    return (w - r);
}

bool buffer_reader::read_info(buffer_info_t& info)
{
    // std::scoped_lock guard(_rdr_mutex);

    info.ptr = _buffer->read_ptr(_read_index);
    info.n_items = items_available();
    info.item_size = _itemsize; //  _buffer->item_size();
    info.total_items = _total_read;

    return true;
}

/**
 * @brief Return the tags associated with this buffer
 *
 * @param num_items Number of items that will be associated with the work call, and
 * thus return the tags from the current read pointer to this specified number of
 * items
 * @return std::vector<tag_t> Returns the vector of tags
 */
std::vector<tag_t> buffer_reader::get_tags(size_t num_items)
{
    std::scoped_lock guard(*(_buffer->mutex()));
    double relative_rate = (double)_itemsize / (double)_buffer->item_size();

    // Find all the tags from total_read to total_read+offset
    std::vector<tag_t> ret;
    for (auto& t : _buffer->tags()) {
        uint64_t new_offset = t.offset();
        if (relative_rate != 1.0) {
            new_offset = t.offset() / relative_rate;
        }
        if (new_offset >= total_read() && new_offset < total_read() + num_items) {
            ret.push_back(t);

            if (relative_rate != 1.0) {
                ret[ret.size() - 1].set_offset(new_offset);
            }
        }
    }

    return ret;
}


std::vector<tag_t> buffer_reader::tags_in_window(const uint64_t item_start,
                                                 const uint64_t item_end)
{
    std::scoped_lock guard(*(_buffer->mutex()));

    double relative_rate = (double)_itemsize / (double)_buffer->item_size();

    std::vector<tag_t> ret;
    for (auto& t : _buffer->tags()) {
        uint64_t new_offset = t.offset();
        if (relative_rate != 1.0) {
            new_offset = t.offset() / relative_rate;
        }
        if (new_offset >= total_read() + item_start &&
            new_offset < total_read() + item_end) {
            ret.push_back(t);

            if (relative_rate != 1.0) {
                ret[ret.size() - 1].set_offset(new_offset);
            }
        }
    }
    return ret;
}

const std::vector<tag_t>& buffer_reader::tags() const
{
    std::scoped_lock guard(*(_buffer->mutex()));
    return _buffer->tags();
}


void buffer_reader::notify_scheduler()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_ALL));
    }
}

void buffer_reader::notify_scheduler_input()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_INPUT));
    }
}

void buffer_reader::notify_scheduler_output()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_OUTPUT));
    }
}


} // namespace gr
