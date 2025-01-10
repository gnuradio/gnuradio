/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gnuradio/block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/buffer_reader_sm.h>
#include <gnuradio/math.h>
#include <algorithm>
#include <stdexcept>

namespace gr {

static long s_buffer_reader_count = 0;

buffer_reader_sptr
buffer_add_reader(buffer_sptr buf, int nzero_preload, block_sptr link, int delay)
{
    if (nzero_preload < 0)
        throw std::invalid_argument("buffer_add_reader: nzero_preload must be >= 0");

    buffer_reader_sptr r;

    if (buf->get_mapping_type() == buffer_mapping_type::double_mapped) {
        r.reset(new buffer_reader(
            buf, buf->index_sub(buf->d_write_index, nzero_preload), link));
        r->declare_sample_delay(delay);
    } else if (buf->get_mapping_type() == buffer_mapping_type::single_mapped) {
        r.reset(new buffer_reader_sm(
            buf, buf->index_sub(buf->d_write_index, nzero_preload), link));
        r->declare_sample_delay(delay);

        // Update reader block history
        buf->update_reader_block_history(link->history(), delay);
        r->d_read_index = buf->d_write_index - nzero_preload;
    }

    buf->d_readers.push_back(r.get());

#ifdef BUFFER_DEBUG
    gr::logger_ptr logger;
    gr::logger_ptr debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "buffer_add_reader");

    std::ostringstream msg;
    msg << " [" << buf.get() << ";" << r.get() << "] buffer_add_reader() nzero_preload "
        << nzero_preload << " -- delay: " << delay << " -- history: " << link->history()
        << " -- RD_idx: " << r->d_read_index;
    GR_LOG_DEBUG(logger, msg.str());
#endif

    return r;
}

buffer_reader::buffer_reader(buffer_sptr buffer, unsigned int read_index, block_sptr link)
    : d_buffer(buffer),
      d_read_index(read_index),
      d_abs_read_offset(0),
      d_link(link),
      d_attr_delay(0)
{
#ifdef BUFFER_DEBUG
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_reader");
#endif

    s_buffer_reader_count++;
}

buffer_reader::~buffer_reader()
{
    d_buffer->drop_reader(this);
    s_buffer_reader_count--;
}

void buffer_reader::declare_sample_delay(unsigned delay)
{
    d_attr_delay = delay;
    d_buffer->d_max_reader_delay = std::max(d_attr_delay, d_buffer->d_max_reader_delay);
}

unsigned buffer_reader::sample_delay() const { return d_attr_delay; }

int buffer_reader::items_available() const
{
    int available = d_buffer->index_sub(d_buffer->d_write_index, d_read_index);

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << d_buffer << ";" << this << "] "
        << "items_available() WR_idx: " << d_buffer->d_write_index
        << " -- WR items: " << d_buffer->nitems_written()
        << " -- RD_idx: " << d_read_index << " -- RD items: " << nitems_read() << " (-"
        << d_attr_delay << ") -- available: " << available;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    return available;
}

const void* buffer_reader::read_pointer()
{
    // Delegate to buffer subclass
    return d_buffer->_read_pointer(d_read_index);
}

void buffer_reader::update_read_pointer(int nitems)
{
    gr::thread::scoped_lock guard(*mutex());

#ifdef BUFFER_DEBUG
    unsigned orig_rd_idx = d_read_index;
#endif

    d_read_index = d_buffer->index_add(d_read_index, nitems);
    d_abs_read_offset += nitems;

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << d_buffer << ";" << this
        << "] update_read_pointer -- orig d_read_index: " << orig_rd_idx
        << " -- nitems: " << nitems << " -- d_read_index: " << d_read_index;
    GR_LOG_DEBUG(d_buffer->d_logger, msg.str());
#endif
}

void buffer_reader::get_tags_in_range(std::vector<tag_t>& v,
                                      uint64_t abs_start,
                                      uint64_t abs_end)
{
    v.clear();
    gr::thread::scoped_lock guard(*mutex());
    auto [lower_bound, upper_bound] = offsets_to_bounds(abs_start, abs_end);
    std::multimap<uint64_t, tag_t>::iterator itr =
        d_buffer->get_tags_lower_bound(lower_bound);
    std::multimap<uint64_t, tag_t>::iterator itr_end =
        d_buffer->get_tags_upper_bound(upper_bound);

    uint64_t item_time;
    while (itr != itr_end) {
        item_time = (*itr).second.offset + d_attr_delay;
        if ((item_time >= abs_start) && (item_time < abs_end)) {
            tag_t t = (*itr).second;
            t.offset += d_attr_delay;
            v.push_back(t);
        }
        itr++;
    }
}

std::tuple<uint64_t, uint64_t> buffer_reader::offsets_to_bounds(uint64_t start,
                                                                uint64_t end) const
{
    uint64_t lower_bound = start - d_attr_delay;
    uint64_t upper_bound = end - d_attr_delay;
    // check for underflow and if so saturate at 0
    if (lower_bound > start)
        lower_bound = 0;
    // check for underflow and if so saturate at 0
    if (upper_bound > end)
        upper_bound = 0;
    return { lower_bound, upper_bound };
}

long buffer_reader_ncurrently_allocated() { return s_buffer_reader_count; }

} /* namespace gr */
